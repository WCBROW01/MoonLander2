/*
 * Map file routines for Moon Lander 2.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include <SDL2/SDL.h>

#include "map.h"

struct ML2_Map {
	char sig[4];
	uint32_t rev;
	uint32_t width, height;
	uint8_t data[];
};

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
ML2_Map *ML2_Map_loadFromFile(const char *path) {
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
	
	// Allocate memory for map
	size_t map_size = map_header.width * map_header.height;
	ML2_Map *map = malloc(sizeof(ML2_Map) + map_size);
	if (!map) {
		SDL_SetError("Failed to load map file %s into memory: not enough memory.", path);
		free(map);
		fclose(map_file);
		return NULL;
	} else {
		*map = map_header;
	}

	// Copy map into memory
	if (fread(map->data, 1, map_size, map_file) != map_size) {
		// Didn't get the correct number of bytes from the file.
		SDL_SetError("Failed to load map file %s into memory: it is an invalid map file.", path);
		free(map);
		fclose(map_file);
		return NULL;
	}

	fclose(map_file);
	return map;
}

// Currently only calls free, here just in case it is needed later.
void ML2_Map_free(ML2_Map *map) {
	free(map);
}

/* Takes a map and coordinates, and gives you the attributes of the tile at
 * those coordinates. Returns the type of tile, and if flip is non-null,
 * it will be filled with information on what direction the tile should
 * be flipped. This value is directly usable with RenderCopyEx. */
int ML2_Map_getTile(ML2_Map *map, uint32_t x, uint32_t y, int *flip) {
	if (!map || x >= map->width || y >= map->height) return -1;
	uint8_t tile_data = map->data[y * map->width + x];
	if (flip) *flip = tile_data >> 6;
	return tile_data & 63;
}
