CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lSDL2

bmptest: bmptest.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f bmptest
