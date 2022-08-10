/**
 * @file
 * @brief Abstractions for using tilesheets with textures.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_TILESHEET_H
#define MOONLANDER_TILESHEET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	SDL_Surface *surface;
	SDL_Texture *texture;
	int tile_width, tile_height;
	int sheet_width, sheet_height;
} TileSheet;

/**
 * @brief Takes an SDL Surface, and the width and height of each tile, and creates a tilesheet.
 * 
 * @param surface The surface to use
 * @param renderer The renderer the tilesheet will render to
 * @param tile_width The width of a single tile
 * @param tile_height The height of a single tile
 * @return The newly created tilesheet
 */
TileSheet *TileSheet_createFromSurface(SDL_Surface *surface, SDL_Renderer *renderer, int tile_width, int tile_height);

/**
 * @brief Takes a Windows bitmap image from RWops, and the width and height of each tile, and creates a tilesheet.
 * @details This does not set the position to 0 beforehand, nor set the position back to its initial value once the operation is complete.
 * 
 * @param src The RWops to use as the source
 * @param freesrc Whether to free the RWops once the tilesheet has been created
 * @param renderer The renderer the tilesheet will render to
 * @param tile_width The width of a single tile
 * @param tile_height The height of a single tile
 * @return The newly created tilesheet
 */
TileSheet *TileSheet_createFromRWops(SDL_RWops *src, SDL_bool freesrc, SDL_Renderer *renderer, int tile_width, int tile_height);

/**
 * @brief Takes the file path of a Windows bitmap image, and the width and height of each tile, and creates a tilesheet.
 * 
 * @param file_path The path to the file containing the tilesheet
 * @param renderer The renderer the tilesheet will render to
 * @param tile_width The width of a single tile
 * @param tile_height The height of a single tile
 * @return The newly created tilesheet
 */
TileSheet *TileSheet_create(const char *file_path, SDL_Renderer *renderer, int tile_width, int tile_height);

/**
 * @brief Frees all the resources for a tilesheet.
 * 
 * @param tilesheet The tilesheet to free
 */
void TileSheet_destroy(TileSheet *tilesheet);

/**
 * @brief Creates a rectangle representing the position of a given tile.
 * @details If an index greater than the last tile is given, a zero-value rectangle will be returned.
 * 
 * @param tilesheet The tilesheet to get the tile from
 * @param index The position of the tile on the tilesheet (left-to-right, top-to-bottom)
 * @return SDL_Rect containing the position and size of the tile
 */
SDL_Rect TileSheet_getTileRect(TileSheet *tilesheet, int index);

/**
 * @brief Get the raw color data of a single pixel in a tile.
 * 
 * @param tilesheet The tilesheet to get the tile from
 * @param index The position of the tile on the tilesheet (left-to-right, top-to-bottom)
 * @param x x-coordinate of the pixel you want
 * @param y y-coordinate of the pixel you want
 * @return Uint32 representing the color of the pixel
 */
Uint32 TileSheet_getPixel(TileSheet *tilesheet, int index, int x, int y);

#ifdef __cplusplus
}
#endif

#endif
