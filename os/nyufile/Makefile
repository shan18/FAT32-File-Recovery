CC=gcc
# CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra

.PHONY: all
all: nyufile

nyufile: nyufile.o disk.o utils.o

nyufile.o: nyufile.c disk.h utils.h

disk.o: disk.c disk.h

utils.o: utils.c utils.h

.PHONY: clean
clean:
	rm -f *.o nyufile