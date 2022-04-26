/*
 * Entrypoint for the game.
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown, Jerrin Redmon
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "lander.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

static void render_screen(SDL_Renderer *renderer, SDL_Texture *texture) {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	SDL_SetRenderTarget(renderer, texture);
}

static void render_title(SDL_Renderer *renderer, SDL_Texture *title) {
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

static void renderbg(SDL_Renderer *renderer) {
	SDL_Rect bg = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_Rect floor = {0, SCREEN_HEIGHT - FLOOR_HEIGHT, SCREEN_WIDTH, FLOOR_HEIGHT};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &bg);
	SDL_SetRenderDrawColor(renderer, 0xC0, 0xC0, 0xC0, 0xFF);
	SDL_RenderFillRect(renderer, &floor);
}

int main(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL failed to initialize! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("Moon Lander", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_RESIZABLE);
	if (!window) {
		fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	// This texture will be used as a buffer for rendering,
	SDL_Texture *render_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

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
		render_title(renderer, title_texture);
		render_screen(renderer, render_texture);
	}

	SDL_DestroyTexture(title_texture);

	// Actual game
	Lander *l = Lander_create(renderer);
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
		renderbg(renderer);
		Lander_render(l);

		render_screen(renderer, render_texture);
	}

	Lander_destroy(l);
	SDL_DestroyTexture(render_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
