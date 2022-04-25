#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <SDL2/SDL.h>

#include "rocket.h"

#define ROCKET_WIDTH 9
#define ROCKET_HEIGHT 14
#define ACCEL 50.0
#define TICKRATE 250
#define MS_PER_TICK (1000 / TICKRATE)
#define GRAVITY 16.2
#define ANIM_RATE 15
#define ANIM_TIME (TICKRATE / ANIM_RATE)
#define NUM_FRAMES 2

#define RTOD(x) ((x) * 180 / M_PI)
#define CMP_ZERO(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)

Uint32 Rocket_physics(Uint32 interval, void *param) {
	Rocket *r = (Rocket*) param;

	r->angle -= r->turning * 0.01;

	if (r->state) {
		r->vel_fuel_x += (r->fast * 2 + 1) * ACCEL / TICKRATE * cosf(r->angle);
		r->vel_fuel_y += (r->fast * 2 + 1) * ACCEL / TICKRATE * sinf(r->angle);
		++r->anim_timer;
		if (r->anim_timer == ANIM_TIME) {
			++r->anim_frame;
			r->anim_frame %= NUM_FRAMES;
			r->anim_timer = 0;
		}
	} else {
		r->vel_fuel_x -= ACCEL / TICKRATE / 2 * CMP_ZERO(r->vel_fuel_x) * fabs(cosf(r->angle));
		r->vel_fuel_y -= ACCEL / TICKRATE / 2 * CMP_ZERO(r->vel_fuel_y) * fabs(sinf(r->angle));

		r->anim_frame = 0;
	}

	// If the rocket has landed, reset velocity and y position to zero.
	if (r->pos_y < 0.0) {
		r->pos_y = 0.0;
	} else if (r->pos_y > 0.0 || r->state) {
		r->vel_grav -= GRAVITY / TICKRATE;
		r->vel_x = r->vel_fuel_x;
		r->vel_y = r->vel_fuel_y + r->vel_grav;
		r->pos_x += r->vel_x / TICKRATE;
		r->pos_y += r->vel_y / TICKRATE;
	} else {
		r->vel_fuel_x = 0.0;
		r->vel_fuel_y = 0.0;
		r->vel_grav = 0.0;
		r->vel_x = 0.0;
		r->vel_y = 0.0;
	}

	return interval;
}

Rocket *Rocket_create(SDL_Renderer *renderer) {
	SDL_Surface *sheet_data = SDL_LoadBMP("rocket.bmp");
	if (!sheet_data) {
		fprintf(stderr, "Failed to load rocket sprite! Error: %s\n", SDL_GetError());
		exit(1);
	}

	Rocket *r = malloc(sizeof(Rocket));
	*r = (Rocket) {
		.renderer = renderer,
		.sprite_sheet = SDL_CreateTextureFromSurface(renderer, sheet_data),
		.sprite_clips = {
			{0, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{ROCKET_WIDTH, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{0, ROCKET_HEIGHT, ROCKET_WIDTH, ROCKET_HEIGHT}
		}
	};

	SDL_FreeSurface(sheet_data);

	Rocket_reset(r);
	r->timer = SDL_AddTimer(MS_PER_TICK, Rocket_physics, r);
	return r;
}

void Rocket_destroy(Rocket *r) {
	SDL_DestroyTexture(r->sprite_sheet);
	free(r);
}

void Rocket_reset(Rocket *r) {
	r->pos_x = 80;
	r->pos_y = 100;
	r->vel_x = 0.0;
	r->vel_y = 0.0;
	r->vel_fuel_x = 0.0;
	r->vel_fuel_y = 0.0;
	r->vel_grav = 0.0;
	r->angle = M_PI / 2;
	r->state = 0;
	r->fast = 0;
	r->turning = 0;
	r->anim_frame = 0;
	r->anim_timer = 0;
}

void Rocket_render(Rocket *r) {
	int s_width, s_height;
	SDL_RenderGetLogicalSize(r->renderer, &s_width, &s_height);

	SDL_Rect rocket_rect = {
		.x = r->pos_x - ROCKET_WIDTH / 2,
		.y = s_height - FLOOR_HEIGHT - ROCKET_HEIGHT + 2 - r->pos_y,
		.w = ROCKET_WIDTH,
		.h = ROCKET_HEIGHT
	};

	const SDL_Point ROCKET_CENTER = {5, 7};

	/* RenderCopyEx uses angle in an entirely different way from how I'm calculating it.
	 * RenderCopyEx takes an angle in degrees and rotates clockwise,
	 * so the angle is inverted and converted to degrees.
	 * The sprite is also stored in the wrong direction,
	 * so 90 degrees are added to the rotation. This will be fixed with the new sprite. */
	SDL_RenderCopyEx(
		r->renderer, r->sprite_sheet,
		&r->sprite_clips[r->state + r->anim_frame], &rocket_rect,
		RTOD(-r->angle) + 90, &ROCKET_CENTER, SDL_FLIP_NONE
	);
}
