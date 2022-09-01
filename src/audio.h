/**
 * @file
 * @brief Audio system for Moon Lander 2. Currently supports mixing up to 4 streams of audio.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_AUDIO_H
#define MOONLANDER_AUDIO_H

typedef struct ML2_AudioSystem ML2_AudioSystem;

ML2_AudioSystem *ML2_AudioSystem_create(
	const char *device,
	SDL_AudioSpec desired,
	SDL_AudioSpec *obtained
);

void ML2_AudioSystem_destroy(ML2_AudioSystem *system);

// Returns -1 on error
int ML2_AudioSystem_addStream(
	ML2_AudioSystem *system,
	SDL_AudioCallback callback,
	void *userdata
);

void ML2_AudioSystem_removeStream(ML2_AudioSystem *system, int stream);

void ML2_AudioSystem_pauseSystem(ML2_AudioSystem *system, SDL_bool pause_on);

void ML2_AudioSystem_pauseStream(ML2_AudioSystem *system, int stream, SDL_bool pause_on);

void ML2_AudioSystem_adjustVolume(ML2_AudioSystem *system, int stream, float volume);

#endif
