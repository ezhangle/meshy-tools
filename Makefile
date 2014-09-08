CC=gcc

GCCFLAGS=-Wall -pedantic -std=c89 -Wextra -O2 -march=native
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
	rotate_mesh	\
	change_basis	\
	normalise_position \
	mesh_axis_size

off_lib: utilities.o
	ar rcs off_utils.a utilities.o

strip_colour: strip_colour.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

strip_normals: strip_normals.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

add_normals: add_normals.c add_normals.h utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

add_colour: add_colour.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

mesh_size:mesh_size.c utilities.o 
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

scale_mesh: scale_mesh.c utilities.o 
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

swap_axes: swap_axes.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

rotate_mesh: rotate_mesh.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

mesh_axis_size: mesh_axis_size.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

change_basis: change_basis.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

normalise_position: normalise_position.c utilities.o
	$(CC) $(CFLAGS) -o $(@) utilities.o $(<) -lm

clean:
	rm -f off_utils.a
	rm -f utilities.o
	rm -f strip_colour
	rm -f strip_normals
	rm -f add_colour
	rm -f add_normals
	rm -f scale_mesh
	rm -f swap_axes
	rm -f mesh_size
	rm -f normalise_position
	rm -f rotate_mesh
	rm -f mesh_axis_size
	rm -f change_basis

install:
	cp strip_colour		${HOME}/bin
	cp strip_normals	${HOME}/bin
	cp add_normals		${HOME}/bin
	cp add_colour		${HOME}/bin
	cp scale_mesh		${HOME}/bin
	cp mesh_size		${HOME}/bin
	cp mesh_axis_size	${HOME}/bin
	cp swap_axes		${HOME}/bin
	cp normalise_position	${HOME}/bin
	cp rotate_mesh		${HOME}/bin
	cp change_basis		${HOME}/bin

