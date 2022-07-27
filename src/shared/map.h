/*
 * Map file routines for Moon Lander 2.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_MAP_H
#define MOONLANDER_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

enum ML2_Map_CollisionAxis {
	ML2_MAP_COLLIDED_X = 1,
	ML2_MAP_COLLIDED_Y = 2
};

typedef struct {
	char sig[4];
	Uint32 rev;
	Uint32 width, height;
	SDL_Color bgcolor;
	TileSheet *tiles;
	Uint8 data[];
} ML2_Map;

/* Load a map from memory. Essentially just a validity check and a cast,
 * but better than doing this yourself. Will set the SDL error state and
 * return NULL if the check fails. This memory should be freed the same
 * way it was acquired. */
ML2_Map *ML2_Map_loadFromMem(void *src);

/* Load the contents of a map file into memory so it can be used in-game.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. */
ML2_Map *ML2_Map_loadFromFile(const char *path, SDL_Renderer *renderer);

// Currently only calls free, here just in case it is needed later.
void ML2_Map_free(ML2_Map *map);

/* Takes a map and coordinates, and gives you the attributes of the tile at
 * those coordinates. Returns the type of tile, and if flip is non-null,
 * it will be filled with information on what direction the tile should
 * be flipped. This value is directly usable with RenderCopyEx. */
int ML2_Map_getTile(ML2_Map *map, Uint32 x, Uint32 y, int *flip);

// Puts dimensions of map into the w and h pointers.
// Deprecated: map dimensions are now publicly exposed!
void ML2_Map_getDim(ML2_Map *map, volatile int *w, volatile int *h);

// Returns whether the rectangle is currently colliding with a tile and the direction.
int ML2_Map_doCollision(ML2_Map *map, const SDL_Rect *r, const SDL_Rect *r_old);

// Render map onto renderer with a given tileset and camera position.
void ML2_Map_render(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos);

#ifdef __cplusplus
}
#endif

#endif
