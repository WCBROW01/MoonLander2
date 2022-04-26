#ifndef MOONLANDER_LANDER_H
#define MOONLANDER_LANDER_H

#include <stdbool.h>

/* arbitrary floor is super temporary,
 * this will be useless if I make anything more advanced.
 * I'll also have to redo the drawing code, but I need to do so anyways. */
#define FLOOR_HEIGHT 16

typedef struct {
	SDL_Renderer *renderer;
	SDL_Texture *sprite_sheet;
	SDL_Rect sprite_clips[4];
	SDL_TimerID timer;
	float pos_x, pos_y, vel_x, vel_y, angle;
	float vel_grav, vel_fuel_x, vel_fuel_y;
	char anim_frame, anim_timer;
	bool state, fast;
	char turning;
} Lander;

Lander *Lander_create(SDL_Renderer *renderer);
void Lander_destroy(Lander *r);
void Lander_reset(Lander *r);
void Lander_render(Lander *r);

#endif
