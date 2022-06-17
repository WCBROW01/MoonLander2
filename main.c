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

#include "lander.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// Game state, may end up in a struct at some point.
static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *render_texture;

static void init_game(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL failed to initialize! Error: %s\n", SDL_GetError());
		exit(1);
	}

	window = SDL_CreateWindow("Moon Lander", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
		exit(1);
	}

	// This texture will be used as a buffer for rendering,
	render_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/* This function frees all game memory and exits the program,
 * so it will never return. */
void exit_game(void) {
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

		SDL_SetRenderTarget(renderer, render_texture);
		SDL_RenderClear(renderer);
		renderbg();
		Lander_render(l);

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
