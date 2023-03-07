GAME_SRC = $(wildcard game/*.c)
GAME_OBJ = $(GAME_SRC:.c=.o)

LIB_SRC = $(wildcard shared/*.c)
LIB_OBJ = $(LIB_SRC:.c=.o)
CFLAGS = -Wall -Wextra -std=c11 `sdl2-config --cflags` -Os -Ishared -flto

# Link SDL statically if building on Windows so you don't need to distribute SDL2.dll
ifeq ($(OS), Windows_NT)
	LDFLAGS = -static `sdl2-config --static-libs`
else
	LDFLAGS = `sdl2-config --libs`
endif

all: moonlander ml2-editor docs

moonlander: $(GAME_OBJ) libML2.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

libML2.a: $(LIB_OBJ)
	ar rcs libML2.a $(LIB_OBJ)

docs: Doxyfile
	doxygen

.PHONY: clean ml2-editor docs
ml2-editor: libML2.a
	cd map_editor && $(MAKE)


clean:
	rm -rf $(GAME_OBJ) $(LIB_OBJ) moonlander libML2.a docs && cd map_editor && $(MAKE) clean
