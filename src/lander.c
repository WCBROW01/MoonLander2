/*
 * Source code for the Lander object.
 * Contributors: Will Brown, Jerrin Redmon
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown, Jerrin Redmon
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

/* Required for M_PI on GCC. SDL normally defines this if it doesn't exist
 * on your platform, so this is only here to make the compiler happy.
 * No GNU extensions are actually used in this code,
 * all source code is compliant with the ISO C11 standard. */
#define _GNU_SOURCE

#include <SDL.h>

#include "tilesheet.h"
#include "lander.h"
#include "map.h"

#define ACCEL 50.0f
#define TICKRATE 250
#define MS_PER_TICK (1000 / TICKRATE)
#define GRAVITY 16.2f
#define ANIM_RATE 15
#define ANIM_TIME (TICKRATE / ANIM_RATE)

#define RTOD(x) ((x) * 180 / M_PI)
#define CMP_ZERO(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)

Uint32 Lander_physics(Uint32 interval, void *param) {
	Lander *l = (Lander*) param;

	l->angle -= l->turning * 0.01f;

	if (l->state && l->fuel_level > 0.0f) {
		// if the fast flag is active (left shift being held) multiply accel by 3
		l->vel_fuel_x += (l->fast * 1.25f + 1.0f) * ACCEL / TICKRATE * SDL_cosf(l->angle);
		l->vel_fuel_y += (l->fast * 1.25f + 1.0f) * ACCEL / TICKRATE * SDL_sinf(l->angle);
		++l->anim_timer;
		if (l->anim_timer == ANIM_TIME) {
			++l->anim_frame;
			l->anim_frame %= l->sprite_sheet->sheet_width - 1;
			l->anim_timer = 0;

			l->fuel_level = l->fuel_level > 0.0f ? l->fuel_level - ACCEL * (l->fast * 1.25f + 1.0f) * 10.0f / TICKRATE : 0.0f;
		}
	} else {
		l->vel_fuel_x -= ACCEL / TICKRATE / 2.0f * CMP_ZERO(l->vel_fuel_x) * SDL_fabs(SDL_cosf(l->angle));
		l->vel_fuel_y -= ACCEL / TICKRATE / 2.0f * CMP_ZERO(l->vel_fuel_y) * SDL_fabs(SDL_sinf(l->angle));
		l->anim_frame = 0;
	}

	if (l->fuel_level < 0.0f) l->fuel_level = 0.0f;

	l->vel_grav -= GRAVITY / TICKRATE;
	l->vel_x = l->vel_fuel_x;
	l->vel_y = l->vel_fuel_y + l->vel_grav;
	l->speed = SDL_fabsf(SDL_roundf(SDL_sqrtf(l->vel_x * l->vel_x + l->vel_y * l->vel_y)));

	SDL_Rect collision_rect_old = {l->pos_x, l->pos_y, LANDER_WIDTH, LANDER_HEIGHT};
	l->pos_x += l->vel_x / TICKRATE;
	l->pos_y += l->vel_y / TICKRATE;
	
	// Make the lander wrap around the map horizontally
	if (l->pos_x > l->map->width * l->map->tiles->tile_width)
		l->pos_x -= l->map->width * l->map->tiles->tile_width;
	else if (l->pos_x < 0)
		l->pos_x += l->map->width * l->map->tiles->tile_width;

	SDL_Rect collision_rect = {l->pos_x, l->pos_y, LANDER_WIDTH, LANDER_HEIGHT};
	int collision = ML2_Map_doCollision(l->map, &collision_rect, &collision_rect_old);
	if (collision & ML2_MAP_COLLIDED_X) {
		l->pos_x -= l->vel_x / TICKRATE;
		l->vel_fuel_x /= 2.0f;
	}
	
	if (collision & ML2_MAP_COLLIDED_Y) {
		l->pos_y -= l->vel_y / TICKRATE;
		l->vel_fuel_y /= 2.0f;
		l->vel_grav = 0.0f;
	}
	
	return interval;
}

Lander *Lander_create(SDL_Renderer *renderer, ML2_Map *map) {
	Lander *l = SDL_malloc(sizeof(Lander));
	*l = (Lander) {
		.renderer = renderer,
		.sprite_sheet = TileSheet_create("Sprites/LunarModule.bmp", renderer, LANDER_WIDTH, LANDER_HEIGHT),
		.map = map
	};

	Lander_reset(l);
	l->timer = SDL_AddTimer(MS_PER_TICK, Lander_physics, l);
	return l;
}

void Lander_destroy(Lander *l) {
	SDL_RemoveTimer(l->timer);
	TileSheet_destroy(l->sprite_sheet);
	SDL_free(l);
}

void Lander_reset(Lander *l) {
	l->pos_x = l->map->start_x;
	l->pos_y = l->map->start_y;
	l->vel_x = 0.0f;
	l->vel_y = 0.0f;
	l->vel_fuel_x = 0.0f;
	l->vel_fuel_y = 0.0f;
	l->fuel_level = l->map->start_fuel;
	l->vel_grav = 0.0f;
	l->speed = 0.0f;
	l->angle = M_PI / 2.0f;
	l->anim_frame = 0;
	l->anim_timer = 0;
}

void Lander_render(Lander *l, SDL_Point *camera_pos) {
	int s_width, s_height;
	SDL_RenderGetLogicalSize(l->renderer, &s_width, &s_height);

	SDL_Rect lander_rect = {
		.x = l->pos_x - camera_pos->x,
		.y = s_height - l->pos_y + camera_pos->y - LANDER_HEIGHT,
		.w = LANDER_WIDTH,
		.h = LANDER_HEIGHT
	};
	
	SDL_Rect sprite = TileSheet_getTileRect(l->sprite_sheet, l->fuel_level > 0.0f ? l->sprite_sheet->sheet_width * l->fast + l->state * (l->anim_frame + 1) : 0);

	/* RenderCopyEx uses angle in an entirely different way from how I'm calculating it.
	 * RenderCopyEx takes an angle in degrees and rotates clockwise,
	 * so the angle is inverted and converted to degrees.
	 * The sprite is also stored in the wrong direction,
	 * so 90 degrees are added to the rotation. This will be fixed with the new sprite. */
	SDL_RenderCopyEx(
		l->renderer, l->sprite_sheet->texture,
		&sprite, &lander_rect,
		RTOD(-l->angle) + 90, NULL, SDL_FLIP_NONE
	);
}
