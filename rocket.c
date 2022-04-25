#include <SDL2/SDL.h>

#include "rocket.h"

#define ROCKET_WIDTH 9
#define ROCKET_HEIGHT 14
#define ACCEL 24.5
#define TICKRATE 250
#define ACCEL_PER_TICK (ACCEL / TICKRATE)
#define MS_PER_TICK (1000 / TICKRATE)
#define ANIM_RATE 15
#define ANIM_TIME (TICKRATE / ANIM_RATE)
#define NUM_FRAMES 2


Uint32 Rocket_physics(Uint32 interval, void *param) {
	Rocket *r = (Rocket*) param;

	if (r->state) {
		r->velocity += ACCEL_PER_TICK;
		++r->anim_timer;
		if (r->anim_timer == ANIM_TIME) {
			++r->anim_frame;
			r->anim_frame %= NUM_FRAMES;
			r->anim_timer = 0;
		}
	} else {
		r->velocity -= ACCEL_PER_TICK;
		r->anim_frame = 0;
	}

	if (r->pos > 0.0) {
		r->pos += r->velocity / TICKRATE;
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
	r->pos = 100;
	r->velocity = 0;
	r->state = 0;
	r->turning = 0;
	r->anim_frame = 0;
	r->anim_timer = 0;
}

void Rocket_render(Rocket *r) {
	int s_width, s_height;
	SDL_RenderGetLogicalSize(r->renderer, &s_width, &s_height);

	SDL_Rect rocket_rect = {
		.x = s_width / 2 - ROCKET_WIDTH/ 2,
		.y = s_height - FLOOR_HEIGHT - ROCKET_HEIGHT + 2 - r->pos,
		.w = ROCKET_WIDTH,
		.h = ROCKET_HEIGHT
	};

	SDL_RenderCopy(r->renderer, r->sprite_sheet, &r->sprite_clips[r->state + r->anim_frame], &rocket_rect);
}
