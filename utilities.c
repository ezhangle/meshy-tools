#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void read_vertex_data(FILE * infile, vector* vertices, vector * normals, long int numverts, int has_normals )
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
	{
		fscanf(infile, " %f %f %f", &vertices[vi].x, &vertices[vi].y, &vertices[vi].z );

		if( has_normals )
			fscanf(infile, " %f %f %f", &normals[vi].x, &normals[vi].y, &normals[vi].z );
	}

	return;
}

void read_face_data( FILE * infile, face* faces, colour * colours, long int numfaces, int has_colour )
{
	long int fi = 0;
	unsigned short int side = 0;

	for(; fi != numfaces; ++fi)
	{
		fscanf(infile, " %d", &faces[fi].sides);
		faces[fi].verts = malloc( faces[fi].sides * sizeof(long int) );

		for( side=0; side != faces[fi].sides; ++side )
			fscanf( infile, " %ld", &faces[fi].verts[side] );

		if( has_colour )
			fscanf( infile, " %d %d %d", &colours[fi].r, &colours[fi].g, &colours[fi].b );
	}

	return;
}

void write_off_file( FILE * outfile, vector * vertices, vector * normals, face * faces, colour * colours, long int numverts, long int numfaces, long int numedges, int write_normals, int write_colours )
{
	long int fi, vi = 0;
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
		fprintf( outfile, "%f %f %f", vertices[vi].x, vertices[vi].y, vertices[vi].z );
		if( write_normals )
			fprintf(outfile, " %f %f %f", normals[vi].x, normals[vi].y, normals[vi].z );
		fprintf(outfile, "\n");
	}

	for(fi=0; fi != numfaces; ++fi)
	{
		fprintf( outfile, "%d", faces[fi].sides );
		for(side = 0; side != faces[fi].sides; ++side)
			fprintf(outfile, " %ld", faces[fi].verts[side] );

		if( write_colours )
			fprintf(outfile, " %d %d %d", colours[fi].r, colours[fi].g, colours[fi].b );

		fprintf(outfile, "\n");
	}

	return;
}
