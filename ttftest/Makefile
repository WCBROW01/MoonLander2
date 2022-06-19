CFLAGS = -Wall -Wextra `sdl2-config --cflags` -O2
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf

ttftest: ttftest.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f ttftest
