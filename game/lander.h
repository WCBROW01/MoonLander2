/**
 * @file
 * @brief Source code for the Lander object.
 * @author Will Brown
 * @author Jerrin Redmon
 * @copyright Licensed under the GNU General Public License v3 (c) 2023 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_LANDER_H
#define MOONLANDER_LANDER_H

#include "tilesheet.h"
#include "map.h"

#define LANDER_WIDTH 16
#define LANDER_HEIGHT 13

/**
 * @brief The data structure holding the state for the lander representing a player.
 */
typedef struct {
	SDL_Renderer *renderer; ///< The renderer the lander is being rendered on
	TileSheet *sprite_sheet; ///< The sprite sheet for the lander
	ML2_Map *map; ///< The map the lander is present on (used for collision)
	float pos_x; ///< x position of the lander
	float pos_y; ///< y position of the lander
	float vel_x; ///< x velocity of the lander
	float vel_y; ///< y velocity of the lander;
	float speed; ///< non-directional speed of the lander as a round number
	float angle; ///< direction of the lander as an angle in radians
	float vel_grav; ///< gravity component of the lander's velocity;
	float vel_fuel_x; ///< fuel component of the lander's velocity, x direction
	float vel_fuel_y; ///< fuel component of the lander's velocity, y direction
	float fuel_level; ///< fuel level of the lander
	char anim_frame; ///< Current frame of animation
	char anim_timer; ///< Time on current frame
	char turning; ///< The direction the player is turning
	SDL_bool state; ///< Whether the player is accelerating
	SDL_bool fast; ///< Whether the player is going fast
} Lander;

/**
 * @brief Create a lander object.
 *
 * @param renderer Renderer to render the lander on
 * @param map Map the lander is present on (used for collision)
 * @return The newly created lander object on the heap
 */
Lander *Lander_create(SDL_Renderer *renderer, ML2_Map *map);

/**
 * @brief Free all resources associated with a lander object.
 *
 * @param l The lander object to destroy
 */
void Lander_destroy(Lander *l);

/**
 * @brief Reset the state of the lander and move it back to the spawnpoint decided by the map.
 *
 * @param l The lander object to reset.
 */
void Lander_reset(Lander *l);

/**
 * @brief Run physics calculations for the current frame
 *
 * @param l The lander object to do physics calculations on
 * @param delta_ms The amount of time since the last frame in milliseconds
 */
void Lander_physics(Lander *l, Uint64 delta_ms);

/**
 * @brief Render the lander on-screen.
 *
 * @param l The lander object to render
 * @param camera_pos The current position of the in-game camera.
 */
void Lander_render(Lander *l, SDL_Point *camera_pos);

#endif
