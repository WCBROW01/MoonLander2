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

Uint32 Rocket_physics(Uint32 interval, void *param) {
	Rocket *r = (Rocket*) param;

	if (r->turning == -1) {
		r->angle += 0.01;
	} else if (r->turning == 1) {
		r->angle -= 0.01;
	}

	if (r->state) {
		r->vel_fuel_x += ACCEL / TICKRATE * cos(r->angle);
		r->vel_fuel_y += ACCEL / TICKRATE * sin(r->angle);
		++r->anim_timer;
		if (r->anim_timer == ANIM_TIME) {
			++r->anim_frame;
			r->anim_frame %= NUM_FRAMES;
			r->anim_timer = 0;
		}
	} else {
		if (r->vel_fuel_x > 1 || r->vel_fuel_x < -1)
			r->vel_fuel_x -= ACCEL / TICKRATE / 2 * cos(r->angle);
		else r->vel_fuel_x = 0;

		if (r->vel_fuel_y > 1 || r->vel_fuel_y < -1)
			r->vel_fuel_y -= ACCEL / TICKRATE / 2 * sin(r->angle);
		else r->vel_fuel_y = 0;
		r->anim_frame = 0;
	}

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

	Rocket *ret = malloc(sizeof(Rocket));
	Rocket r = {
		.renderer = renderer,
		.sprite_sheet = SDL_CreateTextureFromSurface(renderer, sheet_data),
		.sprite_clips = {
			{0, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{ROCKET_WIDTH, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{0, ROCKET_HEIGHT, ROCKET_WIDTH, ROCKET_HEIGHT}
		}
	};

	SDL_FreeSurface(sheet_data);

	*ret = r;
	Rocket_reset(ret);
	r.timer = SDL_AddTimer(MS_PER_TICK, Rocket_physics, ret);
	return ret;
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

	SDL_RenderCopyEx(r->renderer, r->sprite_sheet, &r->sprite_clips[r->state + r->anim_frame], &rocket_rect, RTOD(-r->angle) + 90, &ROCKET_CENTER, SDL_FLIP_NONE);
}
