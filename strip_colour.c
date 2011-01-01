#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void write_colour_file( FILE * colour_file, colour *colours, long int numfaces );

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * colour_dump;

	vertex *vertices = NULL;
	vector *normals = NULL;
	face *faces = NULL;
	colour *colours = NULL;

	unsigned long int numverts, numfaces, numedges;
	int has_normals = 0;
	int has_colour = 0;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> <where-to-dump-colours>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	read_off_header( infile, &has_normals, &has_colour, &numverts, &numfaces, &numedges );

	/* if it claims to have no colour data, exit */
	if( !has_colour )
	{
		fprintf(stderr, "Header does not begin with 'C' - is this a C(N)OFF file?\n");
		fclose(infile);
		exit(0);
	}

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	colours = malloc( numverts * sizeof(colour) );

	if( has_normals )
		normals = malloc( numverts * sizeof(vector) );

	read_vertex_data(infile
		, vertices
		, normals
		, colours
		, numverts
		, has_normals
		, has_colour );

	read_face_data(infile
		, faces
		, numfaces );

	fclose(infile);

	outfile = fopen( argv[2], "w");
	/* !has_colour because we don't want to print the colour data here */
	write_off_file(outfile
		, vertices
		, normals
		, faces
		, colours
		, numverts
		, numfaces
		, numedges
		, has_normals
		, !has_colour );
	fclose(outfile);

	colour_dump = fopen( argv[3], "w");
	if(has_colour && colour_dump != NULL)
	{
		write_colour_file( colour_dump, colours, numverts );
		fclose(colour_dump);
	}
	else if(!has_colour && colour_dump != NULL)

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
		fprintf(colour_file, "%f %f %f\n", colours[fi].r, colours[fi].g, colours[fi].b);

	return;
}
