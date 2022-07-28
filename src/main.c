/*
 * Entrypoint for the game.
 * Contributors: Will Brown, Jerrin Redmon
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include <SDL.h>

#include "lander.h"
#include "tilesheet.h"
#include "tiles.h"
#include "font.h"
#include "map.h"



// Game state, may end up in a struct at some point.
static SDL_Window *window;
static int win_w, win_h, screen_w, screen_h;
static SDL_Renderer *renderer;
static SDL_Texture *render_texture;
static Font *font;
static ML2_Map *map;

/* This function frees all game memory and exits the program,
 * so it will never return. */
void exit_game(void) {
	ML2_Map_free(map);
	SDL_DestroyTexture(render_texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

static void new_render_texture(void) {
	double ratio = win_w > win_h ? (double) win_w / win_h : (double) win_h / win_w;
	screen_w = win_w > win_h ? ratio * 240 : 240;
	screen_h = win_w > win_h ? 240 : ratio * 240;

	SDL_DestroyTexture(render_texture);
	render_texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_TARGET, screen_w, screen_h);
	if (!render_texture) {
		fprintf(stderr, "SDL_CreateTexture: %s\n", SDL_GetError());
		exit(1);
	} else {
		SDL_SetRenderTarget(renderer, render_texture);
	}
}

static void init_game(void) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
		exit(1);
	}

	win_w = 640;
	win_h = 480;
	SDL_CreateWindowAndRenderer(win_w, win_h, SDL_WINDOW_RESIZABLE, &window, &renderer);
	if (!window || !renderer) {
		fprintf(stderr, "SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_SetWindowTitle(window, "Moon Lander");
	SDL_RenderSetVSync(renderer, 1);

	// This texture will be used as a buffer for rendering.
	new_render_texture();
	
	font = Font_create("font.bmp", renderer);
	if (!font) {
		fprintf(stderr, "Font_create: %s\n", SDL_GetError());
		exit(1);
	}

	map = ML2_Map_loadFromFile("test4.ml2", renderer);

	atexit(exit_game);
}

static void render_screen(void) {
	SDL_SetRenderTarget(renderer, NULL);
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
	SDL_Rect title_rect = {screen_w / 2 - w / 2, screen_h / 3 - h / 2, w, h};

	SDL_Rect outline[4];
	for (int i = 0; i < 4; ++i)
		outline[i] = (SDL_Rect) {i, i, screen_w - 2 * i, screen_h - 2 * i};

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0x9C, 0x9C, 0x9C, 0xFF);
	SDL_RenderDrawRects(renderer, outline, 4);
	SDL_RenderCopy(renderer, title, NULL, &title_rect);
}

static SDL_Point get_camera_pos(const SDL_Point *player_pos) {
	SDL_Point camera_pos = {
		player_pos->x - screen_w / 2,
		player_pos->y - screen_h / 2
	};

	if (camera_pos.x < 0) {
		camera_pos.x = 0;
	} else if (camera_pos.x > map->width * 16 - screen_w) {
		camera_pos.x = map->width * 16 - screen_w;
	}

	if (camera_pos.y < 0) {
		camera_pos.y = 0;
	} else if (camera_pos.y > map->height * 16 - screen_h) {
		camera_pos.y = map->height * 16 - screen_h;
	}

	return camera_pos;
}

static void title_screen(void) {
	// Load title screen bitmap
	SDL_Surface *title_bmp = SDL_LoadBMP("ML_title.bmp");
	// 0x00FF00 will be used as a key for transparency.
	SDL_SetColorKey(title_bmp, SDL_TRUE, SDL_MapRGB(title_bmp->format, 0, 255, 0));
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
			} else if (e.type == SDL_WINDOWEVENT) switch (e.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				win_w = e.window.data1;
				win_h = e.window.data2;
				new_render_texture();
				break;
			}
		}

		SDL_SetRenderTarget(renderer, render_texture);
		render_title(title_texture);
		render_screen();
	}

	SDL_DestroyTexture(title_texture);
	if (quit) exit(0);
}

static void render_hud(float speed, float fuel) {
	Font_renderFormatted(font, renderer, NULL, "SPEED %.0f\nFUEL %.0f", speed, fuel);
}

static void game_loop(void) {
	Lander *l = Lander_create(renderer, map);
	SDL_Event e;
	bool quit = false;
	bool using_mouse = false;
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
				using_mouse = false;
				--l->turning;
				break;
			case SDLK_RIGHT:
				using_mouse = false;
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
			} else if (e.type == SDL_WINDOWEVENT) switch (e.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				win_w = e.window.data1;
				win_h = e.window.data2;
				new_render_texture();
				break;
			} else if (e.type == SDL_MOUSEMOTION) {
				using_mouse = true;
			}
		}
		
		SDL_Point lander_point = {l->pos_x, l->pos_y};
		SDL_Point camera_pos = get_camera_pos(&lander_point);
		
		if (using_mouse) {
			int mouse_x, mouse_y;
			SDL_GetMouseState(&mouse_x, &mouse_y);
			mouse_x = mouse_x / ((float) win_w / screen_w);
			mouse_y = screen_h - mouse_y / ((float) win_h / screen_h);
			int lander_screen_x = lander_point.x - camera_pos.x + LANDER_WIDTH / 2;
			int lander_screen_y = lander_point.y - camera_pos.y + LANDER_HEIGHT / 2;
			l->angle = SDL_atan2f(mouse_y - lander_screen_y, mouse_x - lander_screen_x);
		}

#define UNPACK_COLOR(color) (color).r, (color).g, (color).b, (color).a

		// Render black background
		SDL_SetRenderDrawColor(renderer, UNPACK_COLOR(map->bgcolor));
		SDL_RenderClear(renderer);
		
		ML2_Map_render(map, renderer, &camera_pos);
		Lander_render(l, &camera_pos);
		render_hud(l->speed, l->fuel_level);

		render_screen();
	}

	// free lander once loop finishes
	Lander_destroy(l);
}

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	init_game();
	title_screen();
	game_loop();
	return 0;
}
