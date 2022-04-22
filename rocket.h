#ifndef MOONLANDER_ROCKET_H
#define MOONLANDER_ROCKET_H

typedef struct {
	SDL_Renderer *renderer;
	SDL_Texture *sprite_sheet;
	SDL_Rect sprite_clips[3];
	SDL_TimerID timer;
	float pos, velocity;
	int state;

} Rocket;

Rocket *Rocket_create(SDL_Renderer *renderer);
void Rocket_destroy(Rocket *r);
void Rocket_reset(Rocket *r);
void Rocket_render(Rocket *r);

#endif
