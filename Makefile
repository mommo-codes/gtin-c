CC = cc
CFLAGS = -Wall -Wextra -std=c11

gtin: main.c gtin.c gtin.h
	$(CC) $(CFLAGS) -o gtin main.c gtin.c

clean:
	rm -f gtin