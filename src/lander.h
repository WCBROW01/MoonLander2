/**
 * @file
 * @brief Source code for the Lander object.
 * @author Will Brown
 * @author Jerrin Redmon
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_LANDER_H
#define MOONLANDER_LANDER_H

#include "tilesheet.h"
#include "map.h"

#define LANDER_WIDTH 16
#define LANDER_HEIGHT 13

typedef struct {
	SDL_Renderer *renderer;
	TileSheet *sprite_sheet;
	ML2_Map *map;
	SDL_TimerID timer;
	float pos_x, pos_y, vel_x, vel_y, speed, angle;
	float vel_grav, vel_fuel_x, vel_fuel_y, fuel_level;
	char anim_frame, anim_timer;
	char turning;
	SDL_bool state : 1;
	SDL_bool fast : 1;
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
 * @brief Render the lander on-screen.
 * 
 * @param l The lander object to render
 * @param camera_pos The current position of the in-game camera.
 */
void Lander_render(Lander *l, SDL_Point *camera_pos);

#endif
