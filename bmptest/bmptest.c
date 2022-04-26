#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

int main(int argc, char **argv) {
	char *path;

	if (argc < 2) path = "moonL2.bmp";
	else path = argv[1];

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL failed to initialize! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("BMP Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "Window could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!renderer) {
		fprintf(stderr, "Renderer could not be created! Error: %s\n", SDL_GetError());
		return 1;
	}

	// load bitmap from argv[1]
	SDL_Surface *loaded_bmp = SDL_LoadBMP(path);
	if (!loaded_bmp) {
		printf("Unable to load image %s! Error: %s\n", path, SDL_GetError());
	}

	SDL_Texture *bmp_texture = SDL_CreateTextureFromSurface(renderer, loaded_bmp);
	SDL_FreeSurface(loaded_bmp);

	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			}
		}

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, bmp_texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(bmp_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
