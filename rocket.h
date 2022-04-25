#ifndef MOONLANDER_ROCKET_H
#define MOONLANDER_ROCKET_H

#include <stdbool.h>

/* arbitrary floor is super temporary,
 * this will be useless if I make anything more advanced.
 * I'll also have to redo the drawing code, but I need to do so anyways. */
#define FLOOR_HEIGHT 8

typedef struct {
	SDL_Renderer *renderer;
	SDL_Texture *sprite_sheet;
	SDL_Rect sprite_clips[3];
	SDL_TimerID timer;
	float pos_x, pos_y, vel_x, vel_y, angle;
	float vel_grav, vel_fuel_x, vel_fuel_y;
	char anim_frame, anim_timer;
	bool state, fast;
	char turning;
} Rocket;

Rocket *Rocket_create(SDL_Renderer *renderer);
void Rocket_destroy(Rocket *r);
void Rocket_reset(Rocket *r);
void Rocket_render(Rocket *r);

#endif
