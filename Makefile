src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lm -lSDL2

moonlander: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) moonlander
