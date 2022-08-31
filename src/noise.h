/**
 * @file
 * @brief White noise generator.
 * @details This is in a header file because I'm not quite sure where to put it yet.
 * @author Alexey Kutepov
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_NOISE_H
#define MOONLANDER_NOISE_H

#include <assert.h>
#include <stdlib.h>

#define FREQ 44100
#define SAMPLE_DT (1.0f / FREQ)

typedef struct {
	Sint16 current;
	Sint16 next;
	float period;
	float a;
} NoiseGen;

#define LERP(a, b, t) (((b) - (a)) * (t) + (a))

static void white_noise(NoiseGen *gen, Sint16 *stream, size_t stream_len) {
	float gen_step = (1.0f / (gen->period * SAMPLE_DT));

	for (size_t i = 0; i < stream_len; ++i) {
		gen->a += gen_step * SAMPLE_DT;
		stream[i] = LERP(gen->next, gen->current, gen->a);

		if (gen->a >= 1.0f) {
			Sint16 value = (rand() & 1023);
			Sint16 sign = (rand() & 2) - 1;
			gen->current = gen->next;
			gen->next = value * sign;
			gen->a = 0.0f;
		}
	}
}

static void white_noise_callback(void *userdata, Uint8 *stream, int len) {
	white_noise(userdata, (Sint16*) stream, len / 2);
}

#endif
