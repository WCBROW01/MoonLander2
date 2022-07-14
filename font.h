/*
 * Abstractions for using bitmap fonts.
 * Contributors: Will Brown
 * Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_FONT_H
#define MOONLANDER_FONT_H

#include "tilesheet.h"

// This is implemented as an abstraction of tilesheets.
typedef TileSheet Font;

Font *Font_create(const char *file_path, SDL_Renderer *renderer);
void Font_destroy(Font *font);
void Font_renderText(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *text);
void Font_renderFormatted(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *format, ...);

#endif