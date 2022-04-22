src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lSDL2

moonlander: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) moonlander
