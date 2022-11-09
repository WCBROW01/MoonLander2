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

Font *Font_create(const char *file_path, SDL_Renderer *renderer) {
	return TileSheet_create(file_path, renderer, 8, 8, 0);
}

void Font_destroy(Font *font) {
	TileSheet_destroy(font);
}

SDL_Rect Font_renderText(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *text) {
	const SDL_Point orig_p = dst_point == NULL ? (SDL_Point) {0} : *dst_point;
	SDL_Point p = orig_p;

	int max_x = p.x;
	for (const char *c = text; *c != '\0'; ++c) {
		if (*c < 33) {
			switch (*c) {
			case '\b':
				p.x -= 8;
				break;
			case '\t':
				p.x += 32;
				break;
			case '\n':
				p.y += 8;
				// fall through
			case '\r':
				if (p.x > max_x) max_x = p.x;
				p.x = orig_p.x;
				break;
			case ' ':
				p.x += 8;
				break;
			}
		} else {
			SDL_Rect src = TileSheet_getTileRect(font, *c - 33);
			SDL_Rect dst = {.x = p.x, .y = p.y, .w = 8, .h = 8};
			SDL_RenderCopy(renderer, font->texture, &src, &dst);
			p.x += 8;
		}
	}
	
	if (p.x > max_x) max_x = p.x;
	
	return (SDL_Rect) {
		.x = orig_p.x,
		.y = orig_p.y,
		.w = max_x - orig_p.x,
		.h = p.y - orig_p.y + 8
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
