#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {
	if (argc < 5) {
		fprintf(stderr, "Width and height not provided.\n");
		return 1;
	}

	uint32_t width, height;
	char *path = "out.ml2";
	for (char **args = argv; *args != NULL; ++args) {
		if (strcmp(*args, "-w") == 0)
			width = strtol(*++args, NULL, 10);
		else if (strcmp(*args, "-h") == 0)
			height = strtol(*++args, NULL, 10);
		else if (strcmp(*args, "-o") == 0)
			path = *++args;
	}

	const char *SIG = "ML2";
	const uint32_t REV = 1;
	FILE *fp = fopen(path, "w+b");
	if (!fp) {
		perror("Failed to open file");
		return 1;
	}

	// Write header
	fwrite(SIG, sizeof(char), 4, fp);
	fwrite(&REV, sizeof(uint32_t), 1, fp);
	fwrite(&width, sizeof(uint32_t), 1, fp);
	fwrite(&height, sizeof(uint32_t), 1, fp);

	// Write zeros for width and height
	uint8_t zero = 0;
	for (size_t i = 0; i < width * height; ++i)
		fwrite(&zero, 1, 1, fp);

	fclose(fp);
	return 0;
}
