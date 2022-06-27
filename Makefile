src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -Wall -Wextra `sdl2-config --cflags` -O3
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf

moonlander: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) moonlander
