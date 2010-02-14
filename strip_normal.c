#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void write_normal_file( FILE * normal_file, vector *normals, long int numverts );

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * normal_file;

	vertex * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;
	colour * colours = NULL;

	long int numverts, numfaces, numedges;
	int has_normals = 1;
	int has_colours = 0;
	char c;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> <where-to-dump-normals>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	/* if it's a CNOFF file, note the fact and take an extra character */
	c = fgetc(infile);

	if( 'C' == c )
	{
		has_colours = 1;
		c = fgetc(infile);
	}

	if( c != 'N' );
	{
		fprintf(stderr, "Header does not begin with 'N' or 'CN' - is this a (C)NOFF file?\n");
		fclose(infile);
		exit(0);
	}

	/* read OFF */
	fgetc(infile);
	fgetc(infile);
	fgetc(infile);

	fscanf(infile, " %ld", &numverts);	
	fscanf(infile, " %ld", &numfaces);		
	fscanf(infile, " %ld", &numedges);		

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	normals = malloc( numverts * sizeof(vector) );
	if( has_colours )
		colours = malloc( numfaces * sizeof(colour) );

	read_vertex_data(infile, vertices, normals, numverts, has_normals);
	read_face_data(infile, faces, colours, numfaces, has_colours );

	fclose(infile);

	outfile = fopen( argv[2], "w");
	/* !has_colours because we don't want to print the colour data here */
	write_off_file(outfile, vertices, normals, faces, colours, numverts, numfaces, numedges, has_normals, !has_colours );
	fclose(outfile);

	normal_file = fopen( argv[3], "w");
	write_normal_file(normal_file, normals, numverts);
	fclose(normal_file);

	free(vertices);
	free(faces);
	free(normals);

	if( has_colours )
		free(colours);

	return 0;
}

void write_normal_file( FILE * normal_file, vector *normals, long int numverts )
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
		fprintf(normal_file, "%f %f %f\n", normals[vi].x, normals[vi].y, normals[vi].z );

	return;
}
