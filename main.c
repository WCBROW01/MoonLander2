#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "rocket.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

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

	SDL_Window *window = SDL_CreateWindow("Moon Lander", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_Event e;
	Rocket *r = Rocket_create(renderer);

	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_SPACE:
				r->state = 1;
				break;
			case SDLK_LEFT:
				--r->turning;
				break;
			case SDLK_RIGHT:
				++r->turning;
				break;
			case SDLK_r:
				Rocket_reset(r);
				break;
			} else if (e.type == SDL_KEYUP && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_SPACE:
				r->state = 0;
				break;
			case SDLK_LEFT:
				++r->turning;
				break;
			case SDLK_RIGHT:
				--r->turning;
				break;
			}
		}

		SDL_RenderClear(renderer);
		renderbg(renderer);
		Rocket_render(r);
		SDL_RenderPresent(renderer);
	}

	Rocket_destroy(r);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
