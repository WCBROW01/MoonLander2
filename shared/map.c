/**
 * @file
 * @brief Map file routines for Moon Lander 2.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <SDL.h>

#include "tilesheet.h"
#include "tiles.h"
#include "map.h"

// Correct signature is the null-terminated string "ML2"
#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define CORRECT_SIG 0x4D4C3200
#else
#define CORRECT_SIG 0x00324C4D
#endif

#define CURRENT_REV 2

ML2_Map *ML2_Map_create(ML2_Map params, SDL_Renderer *renderer) {
	params.rev = CURRENT_REV;
	if (params.tilesheet_enum) {
		params.tiles = TileSheet_create(TILESHEET_PATHS[params.tilesheet_enum], renderer, 16, 16, TILESHEET_CREATESURFACE);
	}
	if (!params.tiles) return NULL;

	size_t map_size = params.width * params.height;
	ML2_Map *map = SDL_malloc(sizeof(ML2_Map) + map_size);
	if (!map) {
		SDL_SetError("Failed to create map: not enough memory.");
		return NULL;
	}
	
	*map = params;
	memset(map->data, 0, map_size);
	return map;
}

 /* Load the contents of a map from RWops into memory so it can be used in-game.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. */
ML2_Map *ML2_Map_loadFromRWops(SDL_RWops *src, SDL_bool freesrc, SDL_Renderer *renderer) {
	ML2_Map *map = NULL;

	// Copy header into memory and check for validity
	Uint32 loaded_sig;
	ML2_Map map_header;
	if (
		SDL_RWread(src, &loaded_sig, 1, 4) != 4 ||
		loaded_sig != CORRECT_SIG ||
		SDL_RWread(src, &map_header, 1, 12) != 12
	) {
		SDL_SetError("Attempted to load an invalid map.");
		goto done;
	}
	
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
	// Fix byte order of rev 1 header
	map_header.rev = SDL_SwapLE32(map_header.rev);
	map_header.width = SDL_SwapLE32(map_header.width);
	map_header.height = SDL_SwapLE32(map_header.height);
#endif
	
	if (map_header.rev < 2) { // Use old hardcoded values for revision 1 maps.
		map_header.start_x = 5;
		map_header.start_y = 6;
		map_header.start_fuel = 1000;
		map_header.bgcolor = (SDL_Color) {0, 0, 0, 255};
		map_header.tilesheet_enum = TILESHEET_MOON;
		map_header.tiles = TileSheet_create(TILESHEET_PATHS[TILESHEET_MOON], renderer, 16, 16, TILESHEET_CREATESURFACE);
	} else { // Load revision 2 additions
		if (SDL_RWread(src, &map_header.start_x, sizeof(Uint32), 3) != 3) { // get start position, fuel
			SDL_SetError("Attempted to load an invalid map.");
			goto done;
		}
	
		if (SDL_RWread(src, &map_header.bgcolor, 1, 4) != 4) { // get bg color
			SDL_SetError("Attempted to load an invalid map.");
			goto done;
		}
		if (SDL_RWread(src, &map_header.tilesheet_enum, 1, 1) < 1) { // get tilesheet
			SDL_SetError("Attempted to load an invalid map.");
			goto done;
		}
		
		if (map_header.tilesheet_enum) { // use built-in sheet
			if (map_header.tilesheet_enum >= TILESHEET_COUNT) {
				SDL_SetError("Map contains an invalid tilesheet.");
				goto done;
			} else map_header.tiles = TileSheet_create(TILESHEET_PATHS[map_header.tilesheet_enum], renderer, 16, 16, TILESHEET_CREATESURFACE);
		} else { // load embedded sheet
			struct {Uint32 w, h;} dim;
			if (SDL_RWread(src, &dim, sizeof(Uint32), 2) != 2) {
				SDL_SetError("Map contains an invalid tilesheet.");
				goto done;
			};
			map_header.tiles = TileSheet_createFromRWops(src, 0, renderer, dim.w, dim.h, TILESHEET_CREATESURFACE);
		}
		
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
	// Fix byte order of rev 2 header
	map_header.start_x = SDL_SwapLE32(map_header.start_x);
	map_header.start_y = SDL_SwapLE32(map_header.start_y);
	map_header.start_fuel = SDL_SwapLE32(map_header.start_fuel);
#endif
	}
	
	// Allocate memory for map
	size_t map_size = map_header.width * map_header.height;
	map = SDL_malloc(sizeof(ML2_Map) + map_size);
	if (!map) {
		SDL_SetError("Failed to load map into memory: not enough memory.");
		goto done;
	} else {
		*map = map_header;
	}

	// Copy map into memory
	if (SDL_RWread(src, map->data, 1, map_size) != map_size) {
		// Didn't get the correct number of bytes from the source.
		SDL_SetError("Failed to load map into memory: it is an invalid map.");
		SDL_free(map);
		map = NULL;
		goto done;
	}
	
	done:
	if (freesrc) SDL_RWclose(src);
	return map;
}

/* Load the contents of a map from a block of memory so it can be used in-game.
 * This will allocate more memory, since the representation of a map in memory
 * is not strictly a dump of the map.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. */
ML2_Map *ML2_Map_loadFromMem(void *src, int size, SDL_Renderer *renderer) {
	SDL_RWops *rw = SDL_RWFromMem(src, size);
	if (!rw) return NULL;
	
	return ML2_Map_loadFromRWops(rw, 1, renderer);
}

// THIS ONLY WORKS IF FORMAT IS A STRING LITERAL
#define PREFIX_ERROR(format, ...) do { \
	char *error = SDL_strdup(SDL_GetError()); \
	SDL_SetError(format ": %s", __VA_ARGS__, error); \
	SDL_free(error); \
} while (0)

/* Load the contents of a map file into memory so it can be used in-game.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. */
ML2_Map *ML2_Map_loadFromFile(const char *path, SDL_Renderer *renderer) {
	SDL_RWops *src = SDL_RWFromFile(path, "rb");
	if (!src) {
		PREFIX_ERROR("Failed to open map file %s", path);
		return NULL;
	}
	
	return ML2_Map_loadFromRWops(src, 1, renderer);
}

SDL_bool ML2_Map_save(ML2_Map *map, const char *path) {
	SDL_bool success = SDL_TRUE;

	SDL_RWops *rw = SDL_RWFromFile(path, "w");
	if (!rw) {
		PREFIX_ERROR("Failed to save map file %s", path);
		return SDL_FALSE;
	}

	// This intermediate header is here so we don't change byte order in the working copy
	ML2_Map saved_header = {
		// Since it is a working spec, we assume you want the latest one.
		.rev = SDL_SwapLE32(CURRENT_REV),
		.width = SDL_SwapLE32(map->width),
		.height = SDL_SwapLE32(map->height),
		.start_x = SDL_SwapLE32(map->start_x),
		.start_y = SDL_SwapLE32(map->start_y),
		.start_fuel = SDL_SwapLE32(map->start_fuel),
		.bgcolor = map->bgcolor,
		.tiles = map->tiles,
		.tilesheet_enum = map->tilesheet_enum,
	};
	
	if (SDL_RWwrite(rw, "ML2", 1, 4) != 4) {
		success = SDL_FALSE;
		goto done;
	}
	
	if (SDL_RWwrite(rw, &saved_header.rev, sizeof(Uint32), 6) != 6) {
		success = SDL_FALSE;
		goto done;
	}
	
	if (SDL_RWwrite(rw, &saved_header.bgcolor, 1, 4) != 4) {
		success = SDL_FALSE;
		goto done;
	}
	
	if (SDL_RWwrite(rw, &saved_header.tilesheet_enum, 1, 1) != 1) {
		success = SDL_FALSE;
		goto done;
	}
	
	if (!saved_header.tilesheet_enum && SDL_SaveBMP_RW(saved_header.tiles->surface, rw, 0) < 0) {
		success = SDL_FALSE;
		goto done;
	}
	
	size_t map_size = map->width * map->height;
	if (SDL_RWwrite(rw, map->data, 1, map_size) != map_size) {
		success = SDL_FALSE;
		goto done;
	}
	
	done:
	if (!success) PREFIX_ERROR("Failed to save map file %s", path);
	SDL_RWclose(rw);
	return success;
}

// Currently only calls free, here just in case it is needed later.
void ML2_Map_free(ML2_Map *map) {
	if (!map) return;
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

void ML2_Map_setTile(ML2_Map *map, Uint32 x, Uint32 y, int tile, int flip) {
	if (map && x < map->width && y < map->height)
		map->data[y * map->width + x] = tile | flip << 6;
}

// Render map onto renderer with a given tileset and camera position.
void ML2_Map_render(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos) {
	ML2_Map_renderScaled(map, renderer, camera_pos, 1);
}

void ML2_Map_renderScaled(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos, float scale) {
	int render_w, render_h;
	SDL_RenderGetLogicalSize(renderer, &render_w, &render_h);
	if (!render_w || !render_h)
		SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
	
	for (
		int y = camera_pos->y / map->tiles->tile_height / scale;
		y <= (camera_pos->y + render_h) / map->tiles->tile_height / scale;
		++y
	) {
		for (
			int x = camera_pos->x / map->tiles->tile_width / scale;
			x <= (camera_pos->x + render_w) / map->tiles->tile_width / scale;
			++x
		) {
			int flip = 0;
			int tile = ML2_Map_getTile(map, x, y, &flip);
			SDL_Rect src = TileSheet_getTileRect(map->tiles, tile);
			SDL_Rect dst = {
				.x = x * map->tiles->tile_width * scale - camera_pos->x,
				.y = render_h - y * map->tiles->tile_height * scale + camera_pos->y - map->tiles->tile_height * scale,
				.w = map->tiles->tile_width * scale,
				.h = map->tiles->tile_height * scale
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
							SDL_bool collided_left = r_old->x + r_old->w < collider.x && r->x + r->w >= collider.x;
							SDL_bool collided_right = r_old->x >= collider.x && r->x < collider.x;
							SDL_bool collided_top = r_old->y + r_old->h < collider.y && r->y + r->h >= collider.y;
							SDL_bool collided_bottom = r_old->y >= collider.y && r->y < collider.y;
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
