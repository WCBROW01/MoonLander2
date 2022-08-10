/**
 * @file
 * @brief Contains necessary data for using the default set of tiles.
 * This is subject to change, and a new, more dynamic solution may be used later.
 * @author Will Brown
 * @copyright Licensed under the GNU General Public License v3 (c) 2022 Will Brown
 * See LICENSE or <https://www.gnu.org/licenses/>
 */

#ifndef MOONLANDER_TILES_H
#define MOONLANDER_TILES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>

enum tile {
	TILE_NONE,
	TILE_ROCK_0,
	TILE_ROCK_1,
	TILE_ROCK_2,
	TILE_ROCK_3,
	TILE_PLANET,
	TILE_STARS,
	TILE_STARS_PLANET,
	TILE_COUNT
};

enum builtin_tilesheets {
	TILESHEET_CUSTOM,
	TILESHEET_MOON,
	TILESHEET_COUNT
};

static_assert(TILESHEET_COUNT == 2, "Exhaustive handling of tilesets in TILESHEET_PATHS");

static const char *TILESHEET_PATHS[TILESHEET_COUNT] = {
	[TILESHEET_MOON] = "tiles/moon.bmp"
};

#ifdef __cplusplus
}
#endif

#endif
