CC=gcc
# CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra

.PHONY: all
all: nyufile

objects=nyufile.o disk.o recovery.o utils.o

nyufile: $(objects)
	$(CC) $(CFLAGS) -o nyufile $(objects) -l crypto

nyufile.o: nyufile.c disk.h recovery.h utils.h

recovery.o: recovery.c recovery.h disk.h utils.h

disk.o: disk.c disk.h

utils.o: utils.c utils.h

.PHONY: clean
clean:
	rm -f *.o nyufile