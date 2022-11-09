//WORK IN PROGRESS
//NOTE! UNABLE TO COMPILE!


#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>


int initMixer(){
	Mix_Init(0);
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_Mixer couldnt init. Err: %s\n", Mix_GetError());
		return -1;
	}
	return 0;
}


int main(){
	SDL_Init(SDL_INIT_AUDIO);
	initMixer();
	Mix_Chunk *music = Mix_LoadWAV("ML_title-Song.wav");
	int channel = Mix_PlayChannel(-1, music, 0);
	while (Mix_Playing(channel));
	Mix_FreeChunk(music);
	Mix_Quit();
	SDL_Quit();
	return 0;
}
