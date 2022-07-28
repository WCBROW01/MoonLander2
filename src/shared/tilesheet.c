/*
 * Abstractions for using tilesheets with textures.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <SDL.h>

#include "tilesheet.h"

// Takes a Windows bitmap image, and the width and height of each tile, and creates a tilesheet.
TileSheet *TileSheet_create(const char *file_path, SDL_Renderer *renderer, int tile_width, int tile_height) {
	SDL_Surface *surface = SDL_LoadBMP(file_path);
	if (!surface) return NULL;
	// 0x00FF00 will be used as a key for transparency.
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 0));
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) return NULL;
	
	TileSheet *tilesheet = SDL_malloc(sizeof(TileSheet));
	if (!tilesheet) {
		SDL_DestroyTexture(texture);
		SDL_SetError("Failed to allocate memory for tilesheet.");
		return NULL;
	} else {
		int tilesheet_width, tilesheet_height;
		SDL_QueryTexture(texture, NULL, NULL, &tilesheet_width, &tilesheet_height);
		*tilesheet = (TileSheet) {
			.surface = surface,
			.texture = texture,
			.tile_width = tile_width,
			.tile_height = tile_height,
			.sheet_width = tilesheet_width / tile_width,
			.sheet_height = tilesheet_height / tile_height
		};
	}

	return tilesheet;
}

// Frees all the resources for a tilesheet.
void TileSheet_destroy(TileSheet *tilesheet) {
	if (!tilesheet) return;
	SDL_FreeSurface(tilesheet->surface);
	SDL_DestroyTexture(tilesheet->texture);
	SDL_free(tilesheet);
}

/* Creates a rectangle representing the position of a given tile.
 * If an index greater than the last tile is given, a zero-value rectangle will be returned. */
SDL_Rect TileSheet_getTileRect(TileSheet *tilesheet, int index) {
	if (!tilesheet || index >= tilesheet->sheet_width * tilesheet->sheet_height) {
		return (SDL_Rect) {0};
	} else {
		div_t tile = div(index, tilesheet->sheet_width);
		return (SDL_Rect) {
			.x = tile.rem * tilesheet->tile_width,
			.y = tile.quot * tilesheet->tile_height,
			.w = tilesheet->tile_width,
			.h = tilesheet->tile_height
		};
	}
}

Uint32 TileSheet_getPixel(TileSheet *tilesheet, int index, int x, int y) {
	if (!tilesheet || index >= tilesheet->tile_width * tilesheet->tile_height) return 0;

	SDL_Rect tile_rect = TileSheet_getTileRect(tilesheet, index);
	x += tile_rect.x;
	y += tile_rect.y;
	
	int bpp = tilesheet->surface->format->BytesPerPixel;
	Uint8 *p = (Uint8 *)tilesheet->surface->pixels + y * tilesheet->surface->pitch + x * bpp;
	
	switch (bpp) {
	case 1:
		return *p;
	case 2:
		return *(Uint16 *)p;
	case 3:
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		return p[0] << 16 | p[1] << 8 | p[2];
#else
		return p[0] | p[1] << 8 | p[2] << 16;
#endif
	case 4:
		return *(Uint32 *)p;
	default:
		return 0;
	}
}
