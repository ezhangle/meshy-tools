#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void write_colour_file( FILE * colour_file, colour *colours, long int numfaces );

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * colour_dump;

	vertex * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;
	colour * colours = NULL;

	long int numverts, numfaces, numedges;
	int has_normals = 0;
	int has_colours = 1;
	char c;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> <where-to-dump-colours>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	/* if it's a CNOFF file, note the fact and take an extra character */
	c = fgetc(infile);
	if( 'C' != c )
	{
		fprintf(stderr, "Header does not begin with 'C' - is this a C(N)OFF file?\n");
		fclose(infile);
		exit(0);
	}
	if( 'N' == fgetc(infile) )
	{
		has_normals = 1;
		fgetc(infile);
	}
	fgetc(infile);
	fgetc(infile);

	fscanf(infile, " %ld", &numverts);	
	fscanf(infile, " %ld", &numfaces);		
	fscanf(infile, " %ld", &numedges);		


	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	colours = malloc( numfaces * sizeof(colour) );
	if( has_normals )
		normals = malloc( numverts * sizeof(vector) );

	read_vertex_data(infile, vertices, normals, numverts, has_normals);
	read_face_data(infile, faces, colours, numfaces, has_colours );

	fclose(infile);

	outfile = fopen( argv[2], "w");
	/* !has_colours because we don't want to print the colour data here */
	write_off_file(outfile, vertices, normals, faces, colours, numverts, numfaces, numedges, has_normals, !has_colours );
	fclose(outfile);

	colour_dump = fopen( argv[3], "w");
	write_colour_file( colour_dump, colours, numfaces );
	fclose(colour_dump);

	free(vertices);
	free(faces);
	free(colours);
	if( has_normals )
		free(normals);

	return 0;
}

void write_colour_file( FILE * colour_file, colour *colours, long int numfaces )
{
	long int fi = 0;

	for(; fi != numfaces; ++fi)
		fprintf(colour_file, "%d %d %d\n", colours[fi].r, colours[fi].g, colours[fi].b);

	return;
}
