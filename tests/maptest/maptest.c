#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL2/SDL.h>

#include "../../map.h"

static const char SIG[] = "ML2";
static const uint32_t REV = 1;
static const uint32_t WIDTH = 20;
static const uint32_t HEIGHT = 15;

void valid_file(void) {
	printf("Testing whether a valid file works properly.\n");
	// Write file
	printf("Writing file... ");
	FILE *file = fopen("valid.ml2", "w+b");
	fwrite(&SIG, sizeof(char), 4, file);
	fwrite(&REV, sizeof(uint32_t), 1, file);
	fwrite(&WIDTH, sizeof(uint32_t), 1, file);
	fwrite(&HEIGHT, sizeof(uint32_t), 1, file);

	// Write random bytes for the level data
	for (size_t i = 0; i < WIDTH * HEIGHT; ++i) {
		int x = rand();
		fwrite(&x, 1, 1, file);
	}

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
	printf("Writing file... ");
	FILE *file = fopen("invalid_header.ml2", "w+b");
	const char *invalid_sig = "OOF";
	fwrite(invalid_sig, sizeof(char), 4, file);
	printf("DONE\n");
	fclose(file);

	// Load newly created file
	printf("Loading map from file... ");
	ML2_Map *res = ML2_Map_loadFromFile("invalid_header.ml2");
	if (!res) printf("OK\n");
	else printf("FAILED\n");

	remove("invalid_header.ml2");
}

void incomplete_data(void) {
	printf("\nTesting whether incomplete map data is detected\n");
	// Write file
	printf("Writing file... ");
	FILE *file = fopen("incomplete_data.ml2", "w+b");
	fwrite(&SIG, sizeof(char), 4, file);
	fwrite(&REV, sizeof(uint32_t), 1, file);
	fwrite(&WIDTH, sizeof(uint32_t), 1, file);
	fwrite(&HEIGHT, sizeof(uint32_t), 1, file);
	printf("DONE\n");
	fclose(file);

	// Load newly created file
	printf("Loading file into memory... ");
	ML2_Map *res = ML2_Map_loadFromFile("incomplete_data.ml2");
	if (!res) printf("OK\n");
	else printf("FAILED\n");

	remove("incomplete_data.ml2");
}

int main(void) {
	valid_file();
	header_test();
	incomplete_data();
	return 0;
}