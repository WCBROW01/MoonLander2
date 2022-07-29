/*
 * Header for the Lander object.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown, Jerrin Redmon
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

Lander *Lander_create(SDL_Renderer *renderer, ML2_Map *map);
void Lander_destroy(Lander *l);
void Lander_reset(Lander *l);
void Lander_render(Lander *l, SDL_Point *camera_pos);

#endif
