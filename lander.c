/*
 * Source code for the Lander object.
 * Contributors: Will Brown, Jerrin Redmon
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown, Jerrin Redmon
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "lander.h"

#define LANDER_WIDTH 16
#define LANDER_HEIGHT 13
#define ACCEL 50.0f
#define TICKRATE 250
#define MS_PER_TICK (1000 / TICKRATE)
#define GRAVITY 16.2f
#define ANIM_RATE 15
#define ANIM_TIME (TICKRATE / ANIM_RATE)
#define NUM_FRAMES 3

#define RTOD(x) ((x) * 180 / M_PI)
#define CMP_ZERO(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)

Uint32 Lander_physics(Uint32 interval, void *param) {
	Lander *l = (Lander*) param;

	l->angle -= l->turning * 0.01f;

	if (l->state) {
		// if the fast flag is active (left shift being held) multiply accel by 3
		l->vel_fuel_x += (l->fast * 1.25f + 1.0f) * ACCEL / TICKRATE * cosf(l->angle);
		l->vel_fuel_y += (l->fast * 1.25f + 1.0f) * ACCEL / TICKRATE * sinf(l->angle);
		++l->anim_timer;
		if (l->anim_timer == ANIM_TIME) {
			++l->anim_frame;
			l->anim_frame %= NUM_FRAMES;
			l->anim_timer = 0;
		}
	} else {
		l->vel_fuel_x -= ACCEL / TICKRATE / 2 * CMP_ZERO(l->vel_fuel_x) * fabs(cosf(l->angle));
		l->vel_fuel_y -= ACCEL / TICKRATE / 2 * CMP_ZERO(l->vel_fuel_y) * fabs(sinf(l->angle));

		l->anim_frame = 0;
	}

	// If the lander has landed, reset velocity and y position to zero.
	if (l->pos_y <= 0.0) {
		if (l->vel_fuel_y < 0.1f && l->vel_fuel_y > -0.1f) l->vel_fuel_y = 0.0f;
		l->pos_y = 0.0f;
		l->vel_grav = 0.0f;
	}

	l->vel_grav -= GRAVITY / TICKRATE;
	l->vel_x = l->vel_fuel_x;
	l->vel_y = l->vel_fuel_y + l->vel_grav;
	l->speed = fabsf(roundf(sqrtf(l->vel_x * l->vel_x + l->vel_y * l->vel_y)));
	l->pos_x += l->vel_x / TICKRATE;
	l->pos_y += l->vel_y / TICKRATE;

	return interval;
}

Lander *Lander_create(SDL_Renderer *renderer) {
	SDL_Surface *sheet_data = SDL_LoadBMP("Sprites/LunarModule.bmp");
	if (!sheet_data) {
		fprintf(stderr, "Failed to load lander sprite! Error: %s\n", SDL_GetError());
		exit(1);
	}

	Lander *l = malloc(sizeof(Lander));
	*l = (Lander) {
		.renderer = renderer,
		.sprite_sheet = SDL_CreateTextureFromSurface(renderer, sheet_data)
	};

	SDL_FreeSurface(sheet_data);

	Lander_reset(l);
	l->timer = SDL_AddTimer(MS_PER_TICK, Lander_physics, l);
	return l;
}

void Lander_destroy(Lander *l) {
	SDL_RemoveTimer(l->timer);
	SDL_DestroyTexture(l->sprite_sheet);
	free(l);
}

void Lander_reset(Lander *l) {
	l->pos_x = 80;
	l->pos_y = 100;
	l->vel_x = 0.0f;
	l->vel_y = 0.0f;
	l->vel_fuel_x = 0.0f;
	l->vel_fuel_y = 0.0f;
	l->vel_grav = 0.0f;
	l->speed = 0.0f;
	l->angle = M_PI_2;
	l->anim_frame = 0;
	l->anim_timer = 0;
}

void Lander_render(Lander *l) {
	int s_width, s_height;
	SDL_RenderGetLogicalSize(l->renderer, &s_width, &s_height);

	SDL_Rect lander_rect = {
		.x = l->pos_x - LANDER_WIDTH / 2,
		.y = s_height - FLOOR_HEIGHT - LANDER_HEIGHT - l->pos_y,
		.w = LANDER_WIDTH,
		.h = LANDER_HEIGHT
	};
	
	SDL_Rect sprite = {l->state * (l->anim_frame + 1) * LANDER_WIDTH, l->fast * LANDER_HEIGHT, LANDER_WIDTH, LANDER_HEIGHT};

	/* RenderCopyEx uses angle in an entirely different way from how I'm calculating it.
	 * RenderCopyEx takes an angle in degrees and rotates clockwise,
	 * so the angle is inverted and converted to degrees.
	 * The sprite is also stored in the wrong direction,
	 * so 90 degrees are added to the rotation. This will be fixed with the new sprite. */
	SDL_RenderCopyEx(
		l->renderer, l->sprite_sheet,
		&sprite, &lander_rect,
		RTOD(-l->angle) + 90, NULL, SDL_FLIP_NONE
	);
}
