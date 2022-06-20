/*
 * Abstractions for using tilesheets with textures.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <SDL2/SDL.h>

#include "tilesheet.h"

// Takes a Windows bitmap image, and the width and height of each tile, and creates a tilesheet.
TileSheet *TileSheet_create(const char *file_path, SDL_Renderer *renderer, int tile_width, int tile_height) {
	SDL_Surface *surface = SDL_LoadBMP(file_path);
	if (!surface) return NULL;
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	if (!texture) return NULL;
	
	TileSheet *tilesheet = malloc(sizeof(TileSheet));
	if (!tilesheet) {
		SDL_DestroyTexture(texture);
		SDL_Error(SDL_ENOMEM);
		return NULL;
	} else {
		int tilesheet_width, tilesheet_height;
		SDL_QueryTexture(texture, NULL, NULL, &tilesheet_width, &tilesheet_height);
		*tilesheet = (TileSheet) {
			.texture = texture,
			.tile_width = tile_width,
			.tile_height = tile_height,
			.sheet_width = tilesheet_width / tile_width,
			.sheet_height = tilesheet_height / tile_height,
			.num_tiles = tilesheet_width * tilesheet_height
		};
	}

	return tilesheet;
}

// Frees all the resources for a tilesheet.
void TileSheet_destroy(TileSheet *tilesheet) {
	if (!tilesheet) return;
	SDL_DestroyTexture(tilesheet->texture);
	free(tilesheet);
}

/* Creates a rectangle representing the position of a given tile.
 * If an index greater than the last tile is given, a zero-value rectangle will be returned. */
SDL_Rect TileSheet_getTileRect(TileSheet *tilesheet, int index) {
	if (!tilesheet || index >= tilesheet->num_tiles) return (SDL_Rect) {0};
	else return (SDL_Rect) {
		.x = index % tilesheet->sheet_width * tilesheet->tile_width,
		.y = index / tilesheet->sheet_width * tilesheet->tile_height,
		.w = tilesheet->tile_width,
		.h = tilesheet->tile_height
	};
}