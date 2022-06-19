#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL_ttf.h>

int main(int argc, char **argv) {
	char *font_path, *text;

	if (argc < 2) {
		fprintf(stderr, "No font provided.\n");
		return 1;
	} else if (argc < 3) {
		fprintf(stderr, "No text provided.");
		return 1;
	} else {
		font_path = argv[1];
		text = argv[2];
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow("TTF Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface *surface = SDL_GetWindowSurface(window);
	if (TTF_Init() < 0) {
		fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
		return 1;
	}

	TTF_Font *font = TTF_OpenFont(font_path, 32);
	if (!font) {
		fprintf(stderr, "TTF_Font: %s\n", TTF_GetError());
		return 1;
	}

	SDL_Color color = {255, 255, 255, 0};
	SDL_Surface *text_surface = TTF_RenderText_Solid(font, text, color);
	if (!text_surface) {
		fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
		return 1;
	}

	SDL_BlitSurface(text_surface, NULL, surface, NULL);
	SDL_UpdateWindowSurface(window);
	

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
	}

	SDL_FreeSurface(text_surface);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_DestroyWindow(window);
	SDL_Quit();
}