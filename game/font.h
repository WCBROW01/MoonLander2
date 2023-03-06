/**
 * @file
 * @brief Abstractions for using bitmap fonts.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_FONT_H
#define MOONLANDER_FONT_H

#include "tilesheet.h"

/// @brief This is implemented as an abstraction of tilesheets.
typedef struct Font Font;

/**
 * @brief Create a font object
 *
 * @param file_path Path to a Windows bitmap file containing the font
 * @param renderer Renderer to create the font on
 * @return The newly created font object
 */
Font *Font_create(const char *file_path, SDL_Renderer *renderer, int scale);

/**
 * @brief Free all resources associated with a font object
 *
 * @param font The font object to destroy
 */
void Font_destroy(Font *font);

/**
 * @brief Render text using a font.
 *
 * @param font The font to use
 * @param renderer The renderer to render to
 * @param dst_point The start coordinate in the renderer (top-left) for the text.
 * @param text String containing the text to render
 * @return A rectangle containing the bounds of the rendered text
 */
SDL_Rect Font_renderText(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *text);

/**
 * @brief Render a printf-style formatted string using a font.
 *
 * @param font The font to use
 * @param renderer The renderer to render to
 * @param dst_point The start coordinate in the renderer (top-left) for the text.
 * @param format The printf-style format argument
 * @param ... All other printf-style arguments
 * @return A rectangle containing the bounds of the rendered text
 */
SDL_Rect Font_renderFormatted(Font *font, SDL_Renderer *renderer, const SDL_Point *dst_point, const char *format, ...);

#endif
