/**
 * @file
 * @brief Audio system for Moon Lander 2. Currently supports mixing up to 4 streams of audio.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

/* TODO:
 * Handle Errors
 * Log Errors in SDL
 * Decrease size of mixing code if possible
 * Add volume adjustment
 * File loading routines
 * Convert audio formats if necessary
 */

#include <SDL.h>

#include "audio.h"

#define NUM_STREAMS 4

enum ML2_AudioSystem_StreamStatus {
	ML2_AUDIOSYSTEM_IN_USE = 1,
	ML2_AUDIOSYSTEM_PLAYING = 2
};

struct ML2_AudioSystem {
	SDL_AudioSpec spec;
	SDL_AudioCallback callbacks[NUM_STREAMS];
	void *userdata[NUM_STREAMS];
	Uint8 *streams;
	SDL_AudioDeviceID device;
	Uint8 stream_status;
};

#define CLAMP(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

#define SELECT_STREAM(system, x) ((system)->streams + (system)->spec.size * (x))

static void ML2_AudioSystem_callback(void *userdata, Uint8 *stream, int len) {
	ML2_AudioSystem *system = userdata;
	
	// Prepare streams for mixing.
	SDL_memset(stream, system->spec.silence, len);
	for (int i = 0; i < NUM_STREAMS; ++i) {
		if ((system->stream_status & (3 << i * 2)) == (3 << i * 2)) {
			system->callbacks[i](system->userdata[i], SELECT_STREAM(system, i), len);
		} else {
			SDL_memset(SELECT_STREAM(system, i), system->spec.silence, len);
		}
	}
	
	// Mix the streams.
	// This is hell code. I hate this. If I need to debug it, I'm going to cry.
	int audio_bits = SDL_AUDIO_BITSIZE(system->spec.format);
	for (int i = 0; i < len / (audio_bits / 8); ++i) {
		switch (audio_bits) {
			case 8: {
				if (SDL_AUDIO_ISSIGNED(system->spec.format)) {
					// Add all audio sources into buffer
					Sint16 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += ((Sint8 *)SELECT_STREAM(system, j))[i];
					
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT8, SDL_MAX_SINT8);
					
					((Sint8 *)stream)[i] = buf;
				} else {
					// Add all audio sources into buffer
					Sint16 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += ((Uint8 *)SELECT_STREAM(system, j))[i] + SDL_MIN_SINT8;
						
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT8, SDL_MAX_SINT8) - SDL_MIN_SINT8;
					
					((Uint8 *)stream)[i] = buf;
				}
			} break;
			case 16: {
				if ((SDL_AUDIO_ISSIGNED(system->spec.format))) {
					// Add all audio sources into buffer
					Sint32 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += (Sint16) (SDL_AUDIO_ISBIGENDIAN(system->spec.format) ? 
							SDL_SwapBE16(((Uint16 *)SELECT_STREAM(system, j))[i]) :
							SDL_SwapLE16(((Uint16 *)SELECT_STREAM(system, j))[i]));
					
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT16, SDL_MAX_SINT16);
					
					((Sint16 *)stream)[i] = (Sint16) (SDL_AUDIO_ISBIGENDIAN(system->spec.format) ?
						SDL_SwapBE16(buf) :
						SDL_SwapLE16(buf));
				} else {
					// Add all audio sources into buffer
					Sint32 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += SDL_MIN_SINT16 + SDL_AUDIO_ISBIGENDIAN(system->spec.format) ? 
							SDL_SwapBE16(((Uint16 *)SELECT_STREAM(system, j))[i]) :
							SDL_SwapLE16(((Uint16 *)SELECT_STREAM(system, j))[i]);
					
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT16, SDL_MAX_SINT16) - SDL_MIN_SINT16;
					
					((Uint16 *)stream)[i] = SDL_AUDIO_ISBIGENDIAN(system->spec.format) ?
						SDL_SwapBE16(buf) :
						SDL_SwapLE16(buf);
				}
			} break;
			case 32: {
				if (SDL_AUDIO_ISFLOAT(system->spec.format)) {
					// Add all audio sources into buffer
					float buf = 0.0f;
					for (int j = 0; j < NUM_STREAMS; ++j) {
						Uint32 temp = SDL_AUDIO_ISBIGENDIAN(system->spec.format) ? 
							SDL_SwapBE32(((Uint32 *)SELECT_STREAM(system, j))[i]) :
							SDL_SwapLE32(((Uint32 *)SELECT_STREAM(system, j))[i]);
						buf += *(float *)&temp;
					}
					
					// Perform overflow clipping
					buf = CLAMP(buf, -1.0, 1.0);
					
					((Uint32 *)stream)[i] = SDL_AUDIO_ISBIGENDIAN(system->spec.format) ?
						SDL_SwapBE32(*(Uint32 *)&buf) :
						SDL_SwapLE32(*(Uint32 *)&buf);
				} else if ((SDL_AUDIO_ISSIGNED(system->spec.format))) {
					// Add all audio sources into buffer
					Sint64 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += (Sint32) (SDL_AUDIO_ISBIGENDIAN(system->spec.format) ? 
							SDL_SwapBE32(((Uint32 *)SELECT_STREAM(system, j))[i]) :
							SDL_SwapLE32(((Uint32 *)SELECT_STREAM(system, j))[i]));
					
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT32, SDL_MAX_SINT32);
					
					((Sint32 *)stream)[i] = (Sint32) (SDL_AUDIO_ISBIGENDIAN(system->spec.format) ?
						SDL_SwapBE32(buf) :
						SDL_SwapLE32(buf));
				} else {
					// Add all audio sources into buffer
					Sint64 buf = 0;
					for (int j = 0; j < NUM_STREAMS; ++j)
						buf += SDL_MIN_SINT32 + SDL_AUDIO_ISBIGENDIAN(system->spec.format) ? 
							SDL_SwapBE32(((Uint32 *)SELECT_STREAM(system, j))[i]) :
							SDL_SwapLE32(((Uint32 *)SELECT_STREAM(system, j))[i]);
					
					// Perform overflow clipping
					buf = CLAMP(buf, SDL_MIN_SINT32, SDL_MAX_SINT32) - SDL_MIN_SINT32;
					
					((Uint32 *)stream)[i] = SDL_AUDIO_ISBIGENDIAN(system->spec.format) ?
						SDL_SwapBE32(buf) :
						SDL_SwapLE32(buf);
				}
			} break;
		}
	}
}

ML2_AudioSystem *ML2_AudioSystem_create(
	const char *device,
	SDL_AudioSpec desired,
	SDL_AudioSpec *obtained
)
{
	ML2_AudioSystem *system = SDL_malloc(sizeof(ML2_AudioSystem));
	desired.callback = ML2_AudioSystem_callback;
	desired.userdata = system;
	
	system->stream_status = 0;
	system->device = SDL_OpenAudioDevice(device, 0, &desired, &system->spec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	system->streams = SDL_malloc(system->spec.size * NUM_STREAMS);
	
	if (obtained) *obtained = system->spec;
	return system;
}

void ML2_AudioSystem_destroy(ML2_AudioSystem *system) {
	SDL_CloseAudioDevice(system->device);
	SDL_free(system->streams);
	SDL_free(system);
}

// Returns -1 on error
int ML2_AudioSystem_addStream(
	ML2_AudioSystem *system,
	SDL_AudioCallback callback,
	void *userdata
)
{
	for (int i = 0; i < NUM_STREAMS; ++i) {
		if (!(system->stream_status & (1 << i * 2))) {
			system->callbacks[i] = callback;
			system->userdata[i] = userdata;
			system->stream_status &= ~(2 << i * 2);
			system->stream_status |= (1 << i * 2);
			return i;
		}
	}
	
	return -1;
}

void ML2_AudioSystem_removeStream(ML2_AudioSystem *system, int stream) {
	system->stream_status &= ~(3 << stream * 2);
}

void ML2_AudioSystem_pauseSystem(ML2_AudioSystem *system, SDL_bool pause_on) {
	SDL_PauseAudioDevice(system->device, pause_on);
}

void ML2_AudioSystem_pauseStream(ML2_AudioSystem *system, int stream, SDL_bool pause_on) {
	if (pause_on) system->stream_status &= ~(2 << stream * 2);
	else system->stream_status |= 2 << stream * 2;
}
