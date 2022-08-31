game_src = $(wildcard src/*.c)
game_obj = $(game_src:.c=.o)

CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags` -O3 -Isrc/shared -fpic -fno-strict-aliasing

# Link SDL statically if building on Windows so you don't need to distribute SDL2.dll
ifeq ($(OS), Windows_NT)
	LDFLAGS = -static `sdl2-config --static-libs`
else
	LDFLAGS = `sdl2-config --libs`
endif

moonlander: $(game_obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(GAME_LDFLAGS)

.PHONY: clean
clean:
	rm -f $(game_obj) $(lib_obj) moonlander
