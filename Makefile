game_src = $(wildcard src/*.c)
game_obj = $(game_src:.c=.o)

lib_src = $(wildcard src/shared/*.c)
lib_obj = $(lib_src:.c=.o)

CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags` -O3 -Isrc/shared -fpic
LDFLAGS = `sdl2-config --libs`

moonlander: $(game_obj) libML2.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(GAME_LDFLAGS)

libML2.a: $(lib_obj)
	ar rcs libML2.a $(lib_obj)

.PHONY: clean
clean:
	rm -f $(game_obj) $(lib_obj) libML2.a moonlander
