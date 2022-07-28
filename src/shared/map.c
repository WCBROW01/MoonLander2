/*
 * Map file routines for Moon Lander 2.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>

#include <SDL.h>

#include "tilesheet.h"
#include "tiles.h"
#include "map.h"

/* Load a map from memory. Essentially just a validity check and a cast,
 * but better than doing this yourself. Will set the SDL error state and
 * return NULL if the check fails. This memory should be freed the same
 * way it was acquired. */
ML2_Map *ML2_Map_loadFromMem(void *src) {
	ML2_Map *map = src;

	if (!map || SDL_strcmp(map->sig, "ML2") != 0) {
		SDL_SetError("Attempted to load invalid map from memory.");
		return NULL;
	}
	
	return map;
}

/* Load the contents of a map file into memory so it can be used in-game.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. */
 
// TODO: Use goto for error handling.
// TODO: Use RWops instead of FILE pointers.
// TODO: Implement embedded tilesheet loading.
ML2_Map *ML2_Map_loadFromFile(const char *path, SDL_Renderer *renderer) {
	FILE *map_file = fopen(path, "rb");
	if (!map_file) {
		SDL_SetError("Failed to open map file %s: %s", path, strerror(errno));
		return NULL;
	}

	// Copy header into memory and check for validity
	ML2_Map map_header;
	if (
		fread(&map_header, 1, 16, map_file) != 16 ||
		SDL_strcmp(map_header.sig, "ML2") != 0
	) {
		SDL_SetError("%s is an invalid map file.", path);
		fclose(map_file);
		return NULL;
	}

#if SDL_BYTEORDER != SDL_LIL_ENDIAN
	// Fix byte order of header
	map_header.rev = SDL_SwapLE32(map_header.rev);
	map_header.width = SDL_SwapLE32(map_header.width);
	map_header.height = SDL_SwapLE32(map_header.height);
#endif
	
	// Load tilesheet (added in revision 2, custom sheets not supported yet)
	if (map_header.rev < 2) { // revision 1 assumes the moon sheet and black bg
		map_header.bgcolor = (SDL_Color) {0, 0, 0, 255};
		map_header.tiles = TileSheet_create(TILESHEET_PATHS[TILESHEET_MOON], renderer, 16, 16);
	} else {
		if (fread(&map_header.bgcolor, 1, 4, map_file) != 4) { // get bg color
			SDL_SetError("%s is an invalid map file.", path);
			fclose(map_file);
			return NULL;
		}
	
		Uint8 tilesheet_enum;
		if (fread(&tilesheet_enum, 1, 1, map_file) < 1) {
			SDL_SetError("%s is an invalid map file.", path);
			fclose(map_file);
			return NULL;
		}
		
		if (tilesheet_enum) {
			if (tilesheet_enum >= TILESHEET_COUNT) {
				SDL_SetError("%s contains an invalid tilesheet.", path);
				fclose(map_file);
				return NULL;
			} else map_header.tiles = TileSheet_create(TILESHEET_PATHS[tilesheet_enum], renderer, 16, 16);
		} else {
			SDL_SetError("Custom tilesheets are not supported yet.");
			fclose(map_file);
			return NULL;
		}
	}
	
	// Allocate memory for map
	size_t map_size = map_header.width * map_header.height;
	ML2_Map *map = SDL_malloc(sizeof(ML2_Map) + map_size);
	if (!map) {
		SDL_SetError("Failed to load map file %s into memory: not enough memory.", path);
		SDL_free(map);
		fclose(map_file);
		return NULL;
	} else {
		*map = map_header;
	}

	// Copy map into memory
	if (fread(map->data, 1, map_size, map_file) != map_size) {
		// Didn't get the correct number of bytes from the file.
		SDL_SetError("Failed to load map file %s into memory: it is an invalid map file.", path);
		SDL_free(map);
		fclose(map_file);
		return NULL;
	}

	fclose(map_file);
	return map;
}

// Currently only calls free, here just in case it is needed later.
void ML2_Map_free(ML2_Map *map) {
	TileSheet_destroy(map->tiles);
	SDL_free(map);
}

/* Takes a map and coordinates, and gives you the attributes of the tile at
 * those coordinates. Returns the type of tile, and if flip is non-null,
 * it will be filled with information on what direction the tile should
 * be flipped. This value is directly usable with RenderCopyEx. */
int ML2_Map_getTile(ML2_Map *map, Uint32 x, Uint32 y, int *flip) {
	if (!map || x >= map->width || y >= map->height) return -1;
	Uint8 tile_data = map->data[y * map->width + x];
	if (flip) *flip = tile_data >> 6;
	return tile_data & 63;
}

void ML2_Map_getDim(ML2_Map *map, volatile int *w, volatile int *h) {
	if (!map) return;
	if (w) *w = map->width;
	if (h) *h = map->height;
}

// Render map onto renderer with a given tileset and camera position.
void ML2_Map_render(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos) {
	int render_w, render_h;
	SDL_RenderGetLogicalSize(renderer, &render_w, &render_h);
	if (!render_w || !render_h)
		SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
	
	for (
		int y = camera_pos->y / map->tiles->tile_height;
		y <= (camera_pos->y + render_h) / map->tiles->tile_height;
		++y
	) {
		for (
			int x = camera_pos->x / map->tiles->tile_width;
			x <= (camera_pos->x + render_w) / map->tiles->tile_width;
			++x
		) {
			int flip = 0;
			int tile = ML2_Map_getTile(map, x, y, &flip);
			SDL_Rect src = TileSheet_getTileRect(map->tiles, tile);
			SDL_Rect dst = {
				.x = x * map->tiles->tile_width - camera_pos->x,
				.y = render_h - 1 - y * map->tiles->tile_height + camera_pos->y - map->tiles->tile_height,
				.w = map->tiles->tile_width,
				.h = map->tiles->tile_height
			};
			SDL_RenderCopyEx(renderer, map->tiles->texture, &src, &dst, 0, NULL, flip);
		}
	}
}

// Returns whether the rectangle is currently colliding with a tile and the direction.
int ML2_Map_doCollision(ML2_Map *map, const SDL_Rect *r, const SDL_Rect *r_old) {
	struct {
		SDL_Point point;
		int tile;
		int flip;
	} possible_tiles[4] = {
		{.point = {r->x / map->tiles->tile_width, r->y / map->tiles->tile_width}},
		{.point = {(r->x + r->w) / map->tiles->tile_width, r->y / map->tiles->tile_width}},
		{.point = {r->x / map->tiles->tile_width, (r->y + r->h) / map->tiles->tile_width}},
		{.point = {(r->x + r->w) / map->tiles->tile_width, (r->y + r->h) / map->tiles->tile_width}}
	};

	possible_tiles[0].tile = ML2_Map_getTile(
		map,
		possible_tiles[0].point.x,
		possible_tiles[0].point.y,
		&possible_tiles[0].flip
	);
	
	possible_tiles[1].tile = ML2_Map_getTile(
		map,
		possible_tiles[1].point.x,
		possible_tiles[1].point.y,
		&possible_tiles[1].flip
	);
	
	possible_tiles[2].tile = ML2_Map_getTile(
		map,
		possible_tiles[2].point.x,
		possible_tiles[2].point.y,
		&possible_tiles[2].flip
	);
	
	possible_tiles[3].tile = ML2_Map_getTile(
		map,
		possible_tiles[3].point.x,
		possible_tiles[3].point.y,
		&possible_tiles[3].flip
	);

	for (int i = 0; i < 4; ++i) {
		if (possible_tiles[i].tile != -1) {
			for (int y = 0; y < map->tiles->tile_height; ++y) {
				for (int x = 0; x < map->tiles->tile_width; ++x) {
					if (
						TileSheet_getPixel(map->tiles, possible_tiles[i].tile, x, y) !=
						SDL_MapRGB(map->tiles->surface->format, 0, 255, 0)
					) {
						SDL_Point collider = {
							.x = possible_tiles[i].flip & SDL_FLIP_HORIZONTAL ?
								possible_tiles[i].point.x * map->tiles->tile_width + (map->tiles->tile_width - 1 - x) :
								possible_tiles[i].point.x * map->tiles->tile_width + x,
							.y = possible_tiles[i].flip & SDL_FLIP_VERTICAL ?
								possible_tiles[i].point.y * map->tiles->tile_height + y :
								possible_tiles[i].point.y * map->tiles->tile_height + (map->tiles->tile_height - 1 - y)
						};
						
						if (SDL_PointInRect(&collider, r)) {
							if (!r_old) return ML2_MAP_COLLIDED_X | ML2_MAP_COLLIDED_Y;
							bool collided_left = r_old->x + r_old->w < collider.x && r->x + r->w >= collider.x;
							bool collided_right = r_old->x >= collider.x && r->x < collider.x;
							bool collided_top = r_old->y + r_old->h < collider.y && r->y + r->h >= collider.y;
							bool collided_bottom = r_old->y >= collider.y && r->y < collider.y;
							int result = 0;
							if (collided_left || collided_right) result |= ML2_MAP_COLLIDED_X;
							if (collided_top || collided_bottom) result |= ML2_MAP_COLLIDED_Y;
							return result == 0 ? ML2_MAP_COLLIDED_X | ML2_MAP_COLLIDED_Y : result; // hack
						}
					}
				}
			}
		}
	}

	return 0;
}
