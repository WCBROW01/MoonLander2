/**
 * @file
 * @brief Abstractions for using bitmap fonts.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#include <stdio.h>

#include <SDL.h>

#include "tilesheet.h"
#include "font.h"

struct Font {
    TileSheet *ts;
    int scale;
};

Font *Font_create(const char *file_path, SDL_Renderer *renderer, int scale) {
    Font *ret = malloc(sizeof(Font));
	ret->ts = TileSheet_create(file_path, renderer, 8, 8, 0);
	ret->scale = scale;
	return ret;
}

void Font_destroy(Font *font) {
	TileSheet_destroy(font->ts);
	free(font);
}

SDL_Rect Font_renderText(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *text) {
	const SDL_Point orig_p = dst_point == NULL ? (SDL_Point) {0} : *dst_point;
	SDL_Point p = orig_p;

	int max_x = p.x;
	for (const char *c = text; *c != '\0'; ++c) {
		if (*c < 33) {
			switch (*c) {
			case '\b':
				p.x -= 8 * font->scale;
				break;
			case '\t':
				p.x += 32 * font->scale;
				break;
			case '\n':
				p.y += 8 * font->scale;
				// fall through
			case '\r':
				if (p.x > max_x) max_x = p.x;
				p.x = orig_p.x;
				break;
			case ' ':
				p.x += 8 * font->scale;
				break;
			}
		} else {
			SDL_Rect src = TileSheet_getTileRect(font->ts, *c - 33);
			SDL_Rect dst = {.x = p.x, .y = p.y, .w = 8 * font->scale, .h = 8 * font->scale};
			SDL_RenderCopy(renderer, font->ts->texture, &src, &dst);
			p.x += 8 * font->scale;
		}
	}

	if (p.x > max_x) max_x = p.x;

	return (SDL_Rect) {
		.x = orig_p.x,
		.y = orig_p.y,
		.w = max_x - orig_p.x,
		.h = p.y - orig_p.y + 8 * font->scale
	};
}

SDL_Rect Font_renderFormatted(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *format, ...) {
	va_list ap;
	char *text;
	va_start(ap, format);
	SDL_vasprintf(&text, format, ap);
	va_end(ap);

	SDL_Rect r = Font_renderText(font, renderer, dst_point, text);
	SDL_free(text);
	return r;
}
