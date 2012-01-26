#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void write_normal_file(FILE * normal_file
			, vector *normals	
			, long int numverts );

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * normal_file;

	vertex * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;
	colour * colours = NULL;

	unsigned long int numverts, numfaces, numedges;
	int has_normals = 0;
	int has_colour = 0;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> "
			"<where-to-dump-normals>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	/* if it's a CNOFF file, note the fact and take an extra character */
	read_off_header( infile, &has_normals, &has_colour
				, &numverts, &numfaces, &numedges );

	if( !has_normals )
	{
		fprintf(stderr, "Header does not begin with 'N' or 'CN'"
				" - is this a (C)NOFF file?\n");
		fclose(infile);
		exit(0);
	}

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );

	if(argv[3] != NULL)
		normals = malloc( numverts * sizeof(vector) );

	if( has_colour )
		colours = malloc( numverts * sizeof(colour) );

	read_vertex_data(infile
		, vertices
		, normals
		, colours
		, numverts
		, 1
		, has_colour);

	read_face_data(infile, faces, numfaces);

	fclose(infile);

	outfile = fopen( argv[2], "w");
	write_off_file(outfile
		, vertices
		, normals
		, faces
		, colours
		, numverts
		, numfaces
		, numedges
		, 0
		, has_colour );
	fclose(outfile);

	normal_file = fopen( argv[3], "w");
	if( normal_file != NULL )
	{
		write_normal_file(normal_file, normals, numverts);
		fclose(normal_file);
	}

	free(vertices);
	free(faces);
	free(normals);
	free(colours);

	return 0;
}

void write_normal_file( FILE * normal_file, vector *normals, long int numverts )
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
		fprintf(normal_file, "%f %f %f\n"
			, normals[vi].x, normals[vi].y, normals[vi].z );

	return;
}