CC=gcc
CFLAGS=-pedantic -Wall -ansi

off2noff: off2noff.c	off2noff.h
	$(CC) $(CFLAGS) -lm -o off2noff	off2noff.c

noff2off: noff2off.c	noff2off.h
	$(CC) $(CFLAGS) -lm -o noff2off	noff2off.c

off2coff: off2coff.c	off2coff.h
	$(CC) $(CFLAGS) -lm -o off2coff	off2coff.c

coff2off: coff2off.c	coff2off.h
	$(CC) $(CFLAGS) -lm -o coff2off	coff2off.c

clean:
	rm off2noff
	rm noff2off
	rm off2coff
	rm coff2off
