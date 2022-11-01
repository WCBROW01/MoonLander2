/**
 * @file
 * @brief Map file routines for Moon Lander 2.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_MAP_H
#define MOONLANDER_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Flags enum for collision result
 */
enum ML2_Map_CollisionAxis {
	ML2_MAP_COLLIDED_X = 1,
	ML2_MAP_COLLIDED_Y = 2
};

/**
 * @brief Map data
 */
typedef struct {
	Uint32 rev; ///< Revision of the spec that this map conforms to
	Uint32 width; ///< Width of the map
	Uint32 height; ///< Height of the map
	Uint32 start_x; ///< x coordinate for player's starting position
	Uint32 start_y; ///< y coordinate for player's starting position
	Uint32 start_fuel; ///< Amount of fuel the player will start with
	SDL_Color bgcolor; ///< Background color
	TileSheet *tiles; ///< Loaded tilesheet for the map
	Uint8 data[]; ///< Tile data
} ML2_Map;

/**
 * @brief Load the contents of a map from RWops into memory so it can be used in-game.
 * @details If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned. 
 * 
 * @param src RWops to load the map data from
 * @param freesrc Whether to free the RWops once the operation is compelete
 * @param renderer Renderer to associate the loaded tilesheet with
 * @return The newly created map object
 */
ML2_Map *ML2_Map_loadFromRWops(SDL_RWops *src, SDL_bool freesrc, SDL_Renderer *renderer);

/**
 * @brief Load the contents of a map from a block of memory so it can be used in-game.
 * @details This will allocate more memory, since the representation of a map in memory
 * is not strictly a dump of the map.
 * If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned.
 * 
 * @param src Pointer to the map's location in memory
 * @param size Size of the buffer
 * @param renderer Renderer to associate the loaded tilesheet with
 * @return The newly created map object
 */
ML2_Map *ML2_Map_loadFromMem(void *src, int size, SDL_Renderer *renderer);

/**
 * @brief Load the contents of a map file into memory so it can be used in-game.
 * @details If there is an error or the loaded map is invalid, the SDL error state
 * will be set and a null pointer will be returned.
 * 
 * @param path Path to the map file
 * @param renderer Renderer to associate the loaded tilesheet with
 * @return The newly created map object
 */
ML2_Map *ML2_Map_loadFromFile(const char *path, SDL_Renderer *renderer);

/**
 * @brief Frees all resources associated with a map.
 * 
 * @param map The map to free
 */
void ML2_Map_free(ML2_Map *map);

/* Takes a map and coordinates, and gives you the attributes of the tile at
 * those coordinates. Returns the type of tile, and if flip is non-null,
 * it will be filled with information on what direction the tile should
 * be flipped. This value is directly usable with RenderCopyEx. */

/**
 * @brief Takes a map and coordinates, and gives you the attributes of the tile at
 * those coordinates.
 * 
 * @param map The map object to retrieve the tile from
 * @param x x-coordinate of the tile
 * @param y y-coordinate of the tile
 * @param flip If non-null, will be filled with information on what direction the tile should be flipped.
 * This value is directly usable with RenderCopyEx.
 * @return int representing the type of tile
 */
int ML2_Map_getTile(ML2_Map *map, Uint32 x, Uint32 y, int *flip);

// Returns whether the rectangle is currently colliding with a tile and the direction.

/**
 * @brief Returns whether the rectangle is currently colliding with a tile and the direction it is colliding in.
 * 
 * @param map The map object to check collision on
 * @param r An AABB of the collision object
 * @param r_old An AABB of the collision object (old position)
 * @return the collision status of the rectangle (values determined using the ML2_Map_CollisionAxis flags enum)
 */
int ML2_Map_doCollision(ML2_Map *map, const SDL_Rect *r, const SDL_Rect *r_old);

/**
 * @brief Render map onto renderer with a given tileset and camera position.
 * 
 * @param map The map to render
 * @param renderer The renderer to render on
 * @param camera_pos The position of the in-game camera
 */
void ML2_Map_render(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos);

/**
 * @brief Render map onto renderer with a given tileset, camera position, and scale factor.
 * 
 * @param map The map to render
 * @param renderer The renderer to render on
 * @param camera_pos The position of the in-game camera
 * @param scale The factor to scale the render by.
 */
void ML2_Map_renderScaled(ML2_Map *map, SDL_Renderer *renderer, SDL_Point *camera_pos, float scale);

#ifdef __cplusplus
}
#endif

#endif
