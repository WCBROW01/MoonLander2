/*
 * Entrypoint for the game.
 * Contributors: Will Brown, Jerrin Redmon
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "lander.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Game state, may end up in a struct at some point.
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *render_texture;
static TTF_Font *font;

static void init_game(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Moon Lander", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "SDL_CreateRenderer: %s\n", SDL_GetError());
		exit(1);
	}

	if (TTF_Init() < 0) {
		fprintf(stderr, "TTF_Init: %s\n", SDL_GetError());
		exit(1);
	}
	// This texture will be used as a buffer for rendering,
	render_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

	font = TTF_OpenFont("PublicPixel.ttf", 8);
}

/* This function frees all game memory and exits the program,
 * so it will never return. */
void exit_game(void) {
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyTexture(render_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	exit(0);
}

static void render_screen(void) {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, render_texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_SetRenderTarget(renderer, render_texture);
}

static void render_title(SDL_Texture *title) {
	int w, h;
	SDL_QueryTexture(title, NULL, NULL, &w, &h);
	w *= 4;
	h *= 4;
	SDL_Rect title_rect = {SCREEN_WIDTH / 2 - w / 2, SCREEN_HEIGHT / 3 - h / 2, w, h};

	SDL_Rect outline[4];
	for (int i = 0; i < 4; ++i)
		outline[i] = (SDL_Rect) {i, i, SCREEN_WIDTH - 2 * i, SCREEN_HEIGHT - 2 * i};

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x9C, 0x9C, 0x9C, 0xFF);
	SDL_RenderDrawRects(renderer, outline, 4);
	SDL_RenderCopy(renderer, title, NULL, &title_rect);
}

static void renderbg(void) {
	SDL_Rect bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_Rect floor = {0, SCREEN_HEIGHT - FLOOR_HEIGHT, SCREEN_WIDTH, FLOOR_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &bg);
	SDL_SetRenderDrawColor(renderer, 0xC0, 0xC0, 0xC0, 0xFF);
	SDL_RenderFillRect(renderer, &floor);
}

static void title_screen(void) {
	// Load title screen bitmap
	SDL_Surface *title_bmp = SDL_LoadBMP("ML_title.bmp");
	SDL_Texture *title_texture = SDL_CreateTextureFromSurface(renderer, title_bmp);
	SDL_FreeSurface(title_bmp);

	// Title screen
	SDL_Event e;
	bool quit = false;
	bool title = false;
	while (!quit && !title){
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_RETURN:
				title = true;
				break;
			}
		}

		SDL_SetRenderTarget(renderer, render_texture);
		render_title(title_texture);
		render_screen();
	}

	SDL_DestroyTexture(title_texture);
	if (quit) exit_game();
}

static SDL_Texture *render_text_to_texture(const char *text, SDL_Color color) {
	SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

static void render_hud(float speed) {
	SDL_Color color = {255, 255, 255, 0};
	char *speed_text;
	int speed_text_len = SDL_asprintf(&speed_text, "SPEED %g", speed);
	SDL_Texture *speed_texture = render_text_to_texture(speed_text, color);
	free(speed_text);
	SDL_Rect speed_rect = {0, 0, 8 * speed_text_len, 8};
	SDL_RenderCopy(renderer, speed_texture, NULL, &speed_rect);
	SDL_DestroyTexture(speed_texture);
}

static void game_loop(void) {
	Lander *l = Lander_create(renderer);
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_SPACE:
				l->state = 1;
				break;
			case SDLK_LSHIFT:
				l->fast = 1;
				break;
			case SDLK_LEFT:
				--l->turning;
				break;
			case SDLK_RIGHT:
				++l->turning;
				break;
			case SDLK_r:
				Lander_reset(l);
				break;
			} else if (e.type == SDL_KEYUP && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				l->state = 0;
				break;
			case SDLK_LSHIFT:
				l->fast = 0;
				break;
			case SDLK_LEFT:
				++l->turning;
				break;
			case SDLK_RIGHT:
				--l->turning;
				break;
			}
		}

		renderbg();
		Lander_render(l);
		render_hud(l->speed);

		render_screen();
	}

	// free lander once loop finishes
	Lander_destroy(l);
}

int main(void) {
	init_game();
	title_screen();
	game_loop();
	exit_game();
}
