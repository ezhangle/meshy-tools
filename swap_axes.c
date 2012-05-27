#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utilities.h"

/*
*	This program swaps the y and z coordinates .npts and .pts files
*/

int main(int argc, char *argv[])
{
	FILE *infile;
	FILE *outfile;

	float x, y, z;
	float nx, ny, nz;

	enum {	XY_SWAP, XZ_SWAP, YZ_SWAP };
	int to_swap = XY_SWAP;
	int has_normals = 0;
	char *format_string = NULL;

	if(argc!=4)
	{
		printf("Syntax is: %s <infile> <swapped-file>"
			" <\"xy\" swap>\n", argv[0]);
		return 0;
	}

	if(!strcmp("xy", argv[3]))
		to_swap = XY_SWAP;
	else if(!strcmp("xz", argv[3]))
		to_swap = XZ_SWAP;
	else if(!strcmp("yz", argv[3]))
		to_swap = YZ_SWAP;

	open_file(&infile, argv[1], "r");
	open_file(&outfile, argv[2], "w");

	has_normals = detect_normals(infile);
	if(has_normals)
		format_string = "%lf %lf %lf";
	else
		format_string = "%lf %lf %lf %lf %lf %lf";

	/* remember to swap the normals as well */
	switch(to_swap)
	{
	case XY_SWAP:
		while(EOF != fscanf(infile, format_string
					, &x, &y, &z, &nx, &ny, &nz))
		{
			fprintf(outfile, format_string
					, y, x, z, ny, nx, nz);
		}
		break;
	case XZ_SWAP:
		while(EOF != fscanf(infile, format_string
					, &x, &y, &z, &nx, &ny, &nz))
		{
			fprintf(outfile, format_string
					, z, y, x, nz, ny, nx);
		}
		break;

	case YZ_SWAP:
		while(EOF != fscanf(infile, format_string
					, &x, &y, &z, &nx, &ny, &nz))
		{
			fprintf(outfile, format_string
					, x, z, y, nx, nz, ny);
		}
		break;
	}

	fclose(infile);
	fclose(outfile);

	return 0;
}
