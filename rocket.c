#include <SDL2/SDL.h>

#include "rocket.h"

#define ROCKET_WIDTH 9
#define ROCKET_HEIGHT 14
#define SIZE_MOD 4
#define FLOOR_HEIGHT 32
#define ACCEL 98.0
#define TICKRATE 100
#define ACCEL_PER_TICK (ACCEL / TICKRATE)
#define MS_PER_TICK (1000 / TICKRATE)


Uint32 Rocket_physics(Uint32 interval, void *param) {
	Rocket *r = (Rocket*) param;

	if (r->state) {
		r->velocity += ACCEL_PER_TICK;
	} else {
		r->velocity -= ACCEL_PER_TICK;
	}

	if (r->pos > 0.0) {
		r->pos += r->velocity / TICKRATE;
	}

	return interval;
}

Rocket *Rocket_create(SDL_Renderer *renderer) {
	SDL_Surface *sheet_data = SDL_LoadBMP("rocket.bmp");

	Rocket *ret = malloc(sizeof(Rocket));
	Rocket r = {
		.renderer = renderer,
		.sprite_sheet = SDL_CreateTextureFromSurface(renderer, sheet_data),
		.sprite_clips = {
			{0, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{ROCKET_WIDTH, 0, ROCKET_WIDTH, ROCKET_HEIGHT},
			{0, ROCKET_HEIGHT, ROCKET_WIDTH, ROCKET_HEIGHT}
		},
		.pos = 200,
		.velocity = 0,
		.state = 0
	};
	SDL_FreeSurface(sheet_data);

	*ret = r;
	r.timer = SDL_AddTimer(MS_PER_TICK, Rocket_physics, ret);
	return ret;
}

void Rocket_destroy(Rocket *r) {
	SDL_DestroyTexture(r->sprite_sheet);
	free(r);
}

void Rocket_reset(Rocket *r) {
	r->pos = 200;
	r->velocity = 0;
	r->state = 0;
}

void Rocket_render(Rocket *r) {
	int s_width, s_height;
	SDL_GetRendererOutputSize(r->renderer, &s_width, &s_height);

	SDL_Rect rocket_rect = {
		.x = s_width / 2 - ROCKET_WIDTH * SIZE_MOD / 2,
		.y = s_height - FLOOR_HEIGHT - ROCKET_HEIGHT * SIZE_MOD + 2 * SIZE_MOD - r->pos,
		.w = ROCKET_WIDTH * SIZE_MOD,
		.h = ROCKET_HEIGHT * SIZE_MOD
	};

	SDL_RenderCopy(r->renderer, r->sprite_sheet, &r->sprite_clips[r->state], &rocket_rect);
}
