#ifndef MOONLANDER_TILES_H
#define MOONLANDER_TILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <SDL2/SDL.h>

//typedef SDL_Rect TileCollision[16];

enum tile {
	TILE_NONE,
	TILE_ROCK_0,
	TILE_ROCK_1,
	TILE_ROCK_2,
	TILE_ROCK_3,
	TILE_EARTH,
	TILE_STARS,
	TILE_STARS_EARTH,
	TILE_COUNT
};

static_assert(TILE_COUNT == 8, "Exhaustive handling of tiles in TILE_COLLISION_BOXES");

static const SDL_Rect TILE_COLLISION_BOXES[TILE_COUNT][16] = {
	[TILE_ROCK_0] = {{0, 0, 16, 16}},
	[TILE_ROCK_1] = {
		{0, 2, 16, 2}, {0, 3, 14, 1}, {0, 4, 13, 1}, {0, 5, 12, 1},
		{0, 6, 11, 1}, {0, 7, 10, 1}, {0, 8, 9, 1}, {0, 9, 8, 1},
		{0, 10, 6, 2}, {0, 11, 5, 2}, {0, 12, 4, 1}, {0, 13, 3, 1},
		{0, 14, 1, 1}
	},
	[TILE_ROCK_2] = {
		{0, 0, 16, 2}, {2, 2, 14, 1}, {4, 3, 12, 1}, {6, 4, 10, 1},
		{7, 5, 9, 1}, {9, 6, 7, 2}, {10, 8, 6, 1}, {11, 9, 5, 2},
		{12, 11, 4, 1}, {13, 12, 3, 2}, {14, 14, 2, 1}
	},
	[TILE_ROCK_3] = {
		{7, 8, 2, 2}, {6, 7, 4, 1}, {6, 6, 6, 1}, {4, 5, 9, 1},
		{3, 4, 10, 1}, {1, 3, 14, 1}, {0, 0, 16, 3}
	}
};

#ifdef __cplusplus
}
#endif

#endif
