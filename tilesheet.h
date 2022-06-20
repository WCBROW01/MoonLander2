#ifndef MOONLANDER_TILESHEET_H
#define MOONLANDER_TILESHEET_H

typedef struct  {
	SDL_Texture *texture;
	int tile_width, tile_height;
	int sheet_width, sheet_height;
	int num_tiles;
} TileSheet;

// Takes a Windows bitmap image, and the width and height of each tile, and creates a tilesheet.
TileSheet *TileSheet_create(const char *file_path, SDL_Renderer *renderer, int tile_width, int tile_height);

// Frees all the resources for a tilesheet.
void TileSheet_destroy(TileSheet *tilesheet);

/* Creates a rectangle representing the position of a given tile.
 * If an index greater than the last tile is given, a zero-value rectangle will be returned. */
SDL_Rect TileSheet_getTileRect(TileSheet *tilesheet, int index);

#endif