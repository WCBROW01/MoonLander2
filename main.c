#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "rocket.h"

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
	//SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_Texture *render_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Load title screen bitmap
	SDL_Surface *title_bmp = SDL_LoadBMP("moonL2.bmp");
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
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, title_texture, NULL, NULL);

		render_screen(renderer, render_texture);
	}

	SDL_DestroyTexture(title_texture);

	// Actual game
	Rocket *r = Rocket_create(renderer);
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
			case SDLK_LSHIFT:
				r->fast = 1;
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
			case SDLK_LSHIFT:
				r->fast = 0;
				break;
			case SDLK_LEFT:
				++r->turning;
				break;
			case SDLK_RIGHT:
				--r->turning;
				break;
			}
		}

		SDL_SetRenderTarget(renderer, render_texture);
		SDL_RenderClear(renderer);
		renderbg(renderer);
		Rocket_render(r);

		render_screen(renderer, render_texture);
	}

	Rocket_destroy(r);
	SDL_DestroyTexture(render_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
