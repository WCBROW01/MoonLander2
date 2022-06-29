#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL.h>

#include "../../shared/map.h"

static const char SIG[] = "ML2";
static const uint32_t REV = 1;
static const uint32_t WIDTH = 20;
static const uint32_t HEIGHT = 15;

void generate_header(FILE *fp) {
	rewind(fp);
	fwrite(&SIG, sizeof(char), 4, fp);
	fwrite(&REV, sizeof(uint32_t), 1, fp);
	fwrite(&WIDTH, sizeof(uint32_t), 1, fp);
	fwrite(&HEIGHT, sizeof(uint32_t), 1, fp);
}

void write_random_level(FILE *fp) {
	for (size_t i = 0; i < WIDTH * HEIGHT; ++i) {
		int x = rand();
		fwrite(&x, 1, 1, fp);
	}
}

void valid_file(void) {
	printf("Testing whether a valid file works properly.\n");
	// Write file
	printf("Writing normal file... ");
	FILE *file = fopen("valid.ml2", "w+b");
	generate_header(file);

	// Write random bytes for the level data
	write_random_level(file);

	printf("DONE\n");
	fclose(file);

	// Load newly created file
	printf("Loading map from file... ");
	ML2_Map *res = ML2_Map_loadFromFile("valid.ml2");
	if (res) printf("OK\n");
	else printf("FAILED\n");

	// Try loading existing struct
	printf("Loading map from memory... ");
	res = ML2_Map_loadFromMem(res);
	if (res) printf("OK\n");
	else printf("FAILED\n");
	ML2_Map_free(res);

	remove("valid.ml2");
}

void header_test(void) {
	printf("\nTesting header verification.\n");
	// Test signature verification
	printf("Writing corrupted file with incorrect signature... ");
	FILE *file = fopen("invalid_header.ml2", "w+b");
	generate_header(file);
	write_random_level(file);
	
	// Corrupt header
	rewind(file);
	const char *invalid_sig = "OOF";
	fwrite(invalid_sig, sizeof(char), 4, file);
	printf("DONE\n");
	fclose(file);

	// Load newly created file
	printf("Testing for failed load... ");
	ML2_Map *res = ML2_Map_loadFromFile("invalid_header.ml2");
	if (!res) printf("OK\n");
	else printf("FAILED\n");

	remove("invalid_header.ml2");
}

void incomplete_header(void) {
	// Test signature verification
	printf("Writing incomplete file (<16 bytes)... ");
	FILE *file = fopen("incomplete_header.ml2", "w+b");
	fwrite(SIG, sizeof(char), 4, file);
	printf("DONE\n");
	fclose(file);
	
	// Laod newly created file
	printf("Testing for failed load... ");
	ML2_Map *res = ML2_Map_loadFromFile("invalid_header.ml2");
	if (!res) printf("OK\n");
	else printf("FAILED\n");
	
	remove("incomplete_header.ml2");
}

void incomplete_data(void) {
	printf("\nTesting whether incomplete map data is detected\n");
	// Write file
	printf("Writing corrupted file... ");
	FILE *file = fopen("incomplete_data.ml2", "w+b");
	generate_header(file);
	printf("DONE\n");
	fclose(file);

	// Load newly created file
	printf("Testing for failed load... ");
	ML2_Map *res = ML2_Map_loadFromFile("incomplete_data.ml2");
	if (!res) printf("OK\n");
	else printf("FAILED\n");

	remove("incomplete_data.ml2");
}

void test_tiles(void) {
	printf("\nWriting test map with valid tile data...");
	// Write header
	FILE *file = fopen("tile_test.ml2", "w+b");
	generate_header(file);
	
	/* 
	 * Write first four tiles as seen here:
	 * 0. Tile 0, no flip.
	 * 1. Tile 1, flip x.
	 * 2. Tile 2, flip y.
	 * 3. Tile 3, flip both x and y.
	 * Both tiles and flip types should be 0, 1, 2, and 3 respectively.
	 */
	uint8_t data[300] = {0x00, 0x41, 0x82, 0xC3, 0x00};
	fwrite(data, 1, sizeof(data), file);
	fclose(file);
	printf("OK\n");
	
	// Load file
	printf("Loading generated file... ");
	ML2_Map *map = ML2_Map_loadFromFile("tile_test.ml2");
	if (map) printf("OK\n");
	else printf("FAILED\n");
	
	// Get tiles
	printf("Checking getTile with NULL flip parameter... ");
	int x = 0, y = 0;
	printf("Got tile %d at coordinate %d,%d\n", ML2_Map_getTile(map, x, y, NULL), x, y);
	printf("Checking test tiles...\n");
	int flip;
	int tile = ML2_Map_getTile(map, x, y, &flip);
	printf("Got tile %d with flip type %d at coordinate %d,%d\n", tile, flip, x, y);
	x = 1;
	tile = ML2_Map_getTile(map, x, y, &flip);
	printf("Got tile %d with flip type %d at coordinate %d,%d\n", tile, flip, x, y);
	x = 2;
	tile = ML2_Map_getTile(map, x, y, &flip);
	printf("Got tile %d with flip type %d at coordinate %d,%d\n", tile, flip, x, y);
	x = 3;
	tile = ML2_Map_getTile(map, x, y, &flip);
	printf("Got tile %d with flip type %d at coordinate %d,%d\n", tile, flip, x, y);
	
	printf("Testing overflow behavior... ");
	int res = ML2_Map_getTile(map, 50, 50, NULL);
	printf(res == -1 ? "OK\n" : "FAILED\n");
	
	ML2_Map_free(map);
	remove("tile_test.ml2");
}

int main(void) {
	valid_file();
	header_test();
	incomplete_header();
	incomplete_data();
	test_tiles();
	return 0;
}
