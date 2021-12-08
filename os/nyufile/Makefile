CC=gcc
# CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra

.PHONY: all
all: nyufile

nyufile: nyufile.o

nyufile.o: nyufile.c

.PHONY: clean
clean:
	rm -f *.o nyufile