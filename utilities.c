#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void read_off_header(FILE * infile
		, int * has_normals
		, int * has_colour
		, unsigned long int *numverts
		, unsigned long int *numfaces
		, unsigned long int *numedges)
{
	char c = fgetc(infile);

	if( 'C' == c )
	{
		c = fgetc(infile);
		*has_colour = 1;
	}

	if( 'N' == c )
	{
		c = fgetc(infile);
		*has_normals = 1;
	}
	
	fgetc(infile);
	fgetc(infile);

	fscanf(infile, "%lu %lu %lu", numverts, numfaces, numedges);

	return;
}

void read_vertex_data(FILE * infile
	, vertex *vertices
	, vector *normals
	, colour *colours
	, long int numverts
	, int has_normals
	, int has_colour)
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
	{
		fscanf(infile, " %f %f %f", &vertices[vi].x
				, &vertices[vi].y, &vertices[vi].z );

		if( has_normals && normals != NULL)
			fscanf(infile, " %f %f %f", &normals[vi].x
				, &normals[vi].y, &normals[vi].z );
		else if (has_normals && normals == NULL)
			fscanf(infile, " %*f %*f %*f");

		if( has_colour && colours != NULL)
			fscanf( infile, " %f %f %f 1", &colours[vi].r
				, &colours[vi].g, &colours[vi].b );
		else if (has_colour && colours == NULL)
			fscanf(infile, " %*f %*f %*f 1");
	}

	return;
}

void read_face_data( FILE * infile
	, face* faces
	, long int numfaces)
{
	long int fi = 0;
	unsigned short int side = 0;

	for(; fi != numfaces; ++fi)
	{
		fscanf(infile, "%d ", &(faces[fi].sides));
		faces[fi].verts = malloc( faces[fi].sides * sizeof(long) );

		for(side=0; side != faces[fi].sides; ++side )
		{
			if( EOF == fscanf( infile, " %ld"
						, &faces[fi].verts[side] ) )
			{
				fprintf(stderr, "face data error, exiting\n");
				exit(1);
			}
		}
	}

	return;
}

void write_off_file( FILE * outfile
		, vertex * vertices
		, vector * normals
		, face * faces
		, colour * colours
		, long int numverts
		, long int numfaces
		, long int numedges
		, int write_normals
		, int write_colours )
{
	long int fi = 0, vi = 0;
	int side;

	if( write_colours )
		fprintf( outfile, "C");

	if( write_normals )
		fprintf( outfile, "N");

	fprintf( outfile, "OFF\n" );

	fprintf( outfile, "%ld %ld %ld\n", numverts, numfaces, numedges );

	/* write the vertex data */
	for(; vi != numverts; ++vi)
	{
		fprintf( outfile, "%f %f %f"
			, vertices[vi].x, vertices[vi].y, vertices[vi].z );

		if( write_normals )
			fprintf(outfile, " %f %f %f", normals[vi].x
				, normals[vi].y, normals[vi].z );

		if( write_colours )
			fprintf(outfile, " %f %f %f"
				, colours[fi].r, colours[fi].g, colours[fi].b );

		fprintf(outfile, "\n");
	}

	for(; fi != numfaces; ++fi)
	{
		fprintf( outfile, "%d", faces[fi].sides );
		if(faces[fi].sides == 0)
			fprintf(stderr, "fucked\n");
		for(side = 0; side != faces[fi].sides; ++side)
			fprintf(outfile, " %ld", faces[fi].verts[side] );

		fprintf(outfile, "\n");
	}

	return;
}
