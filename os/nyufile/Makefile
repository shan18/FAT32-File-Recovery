CC=gcc
# CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra

.PHONY: all
all: nyufile

nyufile: nyufile.o disk.o

nyufile.o: nyufile.c disk.h

disk.o: disk.c disk.h

.PHONY: clean
clean:
	rm -f *.o nyufile