CC=gcc
GCCFLAGS=-Wall -pedantic -std=c89 -Wextra -g
TCCFLAGS=-Wall -Wunsupported
CLANGFLAGS=-ansi -pedantic -analyze
CFLAGS=$(GCCFLAGS)

all: strip_colour strip_normals add_normals add_colour orient_norms

orient_norms: orient_norms.c utilities.o
	$(CC) -lm -o orient_norms orient_norms.c utilities.o

strip_colour: strip_colour.c utilities.o
	$(CC) $(CFLAGS) -o strip_colour strip_colour.c utilities.o

strip_normals: strip_normals.c utilities.o
	$(CC) $(CFLAGS) -o strip_normals strip_normals.c utilities.o

add_normals: add_normals.c utilities.o
	$(CC) $(CFLAGS) -lm -o add_normals add_normals.c utilities.o

add_colour: add_colour.c utilities.o
	$(CC) $(CFLAGS) -lm -o add_colour add_colour.c utilities.o

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c


clean:
	rm utilities.o
	rm strip_colours
	rm strip_normals
	rm add_normals
	rm strip_normals
	rm orient_normals
