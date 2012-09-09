CC=gcc

GCCFLAGS=-Wall -pedantic -std=c89 -Wextra -g
TCCFLAGS=-Wall -Wunsupported
CLANGFLAGS=-ansi -pedantic -analyze
PCCFLAGS=

CFLAGS=$(GCCFLAGS)

all:	strip_colour	\
	strip_normals	\
	add_normals	\
	add_colour	\
	scale_mesh	\
	mesh_size	\
	swap_axes	\
	off_lib		\
	orient_norms

off_lib: utilities.o
	ar rcs off_utils.a utilities.o

orient_norms: orient_norms.c utilities.o
	$(CC) -o orient_norms orient_norms.c utilities.o -lm

strip_colour: strip_colour.c utilities.o
	$(CC) $(CFLAGS) -o strip_colour strip_colour.c utilities.o

strip_normals: strip_normals.c utilities.o
	$(CC) $(CFLAGS) -o strip_normals strip_normals.c utilities.o

add_normals: add_normals.c utilities.o
	$(CC) $(CFLAGS) -o add_normals add_normals.c utilities.o -lm

add_colour: add_colour.c utilities.o
	$(CC) $(CFLAGS) -o add_colour add_colour.c utilities.o

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

mesh_size:mesh_size.c utilities.o 
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<)

scale_mesh: scale_mesh.c utilities.o 
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<)

swap_axes: swap_axes.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<)

normalise_position: normalise_position.c utilities.o 
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<)

clean:
	rm -f off_utils.a
	rm -f utilities.o
	rm -f strip_colour
	rm -f strip_normals
	rm -f add_colour
	rm -f add_normals
	rm -f orient_normals
