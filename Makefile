CC=gcc
CFLAGS=-pedantic -Wall -std=c89

all: strip_colours strip_normals add_normals add_colour

strip_colours: strip_colour.o utilities.o
	$(CC) $(CFLAGS) -o strip_colours strip_colour.o utilities.o

strip_colour.o: strip_colour.c utilities.h
	$(CC) $(CFLAGS) -c strip_colour.c

strip_normals: strip_normals.o utilities.o
	$(CC) $(CFLAGS) -o strip_normals strip_normals.o utilities.o

strip_normal.o: strip_normal.c utilities.h
	$(CC) $(CFLAGS) -c strip_normals.c

add_normals: add_normals.o utilities.o
	$(CC) $(CFLAGS) -lm -o add_normals add_normals.o utilities.o

add_normals.o: add_normals.c add_normals.h utilities.h
	$(CC) $(CFLAGS) -c add_normals.c

add_colour: add_colour.c utilities.o
	$(CC) $(CFLAGS) -lm -o add_colour add_colour.c utilities.o

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c


clean:
	rm *.o
	rm strip_colours
	rm strip_normals
	rm add_normals
