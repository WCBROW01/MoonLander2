game_src = $(wildcard src/*.c)
game_obj = $(game_src:.c=.o)

CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags` -O3 -Isrc/shared -flto

# Link SDL statically if building on Windows so you don't need to distribute SDL2.dll
ifeq ($(OS), Windows_NT)
	LDFLAGS = -static `sdl2-config --static-libs`
else
	LDFLAGS = `sdl2-config --libs`
endif

moonlander: $(game_obj)

.PHONY: clean
clean:
	rm -f $(game_obj) moonlander
