CFLAGS=-Wall -Wextra `sdl2-config --cflags` -g
LDFLAGS=`sdl2-config --libs`

maptest: maptest.c ../../map.c
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f maptest