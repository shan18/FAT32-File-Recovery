CC=gcc
CFLAGS=-g -pedantic -std=gnu99 -Wall -Werror -Wextra

.PHONY: all
all: run

objects=run.o disk.o recovery.o utils.o

run: $(objects)
	$(CC) $(CFLAGS) -o run $(objects) -l crypto

run.o: run.c disk.h recovery.h utils.h

recovery.o: recovery.c recovery.h disk.h utils.h

disk.o: disk.c disk.h

utils.o: utils.c utils.h

.PHONY: clean
clean:
	rm -f *.o run
