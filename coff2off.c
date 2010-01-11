#include <stdio.h>
#include <stdlib.h>
#include "coff2off.h"

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * colour_dump;

	vector * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;

	long int numverts, numfaces, numedges;
	int has_normals = 0;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> <where-to-dump-colours>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );
	colour_dump = fopen( argv[3], "w");

	/* if it's a NOFF file, take an extra character */
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

	if( has_normals )
		normals = malloc( numverts * sizeof(vector) );

	vertices = malloc( numverts * sizeof(vector) );
	faces = malloc( numfaces * sizeof(face) );

	read_vertex_data(infile, vertices, normals, numverts, has_normals);
	read_face_data(infile, colour_dump, faces, numfaces);
	fclose(infile);
	fclose(colour_dump);

	outfile = fopen( argv[2], "w");
	write_off_file(outfile, vertices, normals, faces, numverts, numfaces, numedges, has_normals );
	fclose(outfile);

	return 0;
}

void read_vertex_data(FILE * infile, vector* vertices, vector * normals, long int numverts, int has_normals )
{
	long int vi = 0;
	double nx, ny, nz;

	for(; vi != numverts; ++vi)
	{
		fscanf(infile, " %lf %lf %lf", &vertices[vi].x, &vertices[vi].y, &vertices[vi].z );
		if( has_normals )
			fscanf(infile, " %lf %lf %lf", &normals[vi].x, &normals[vi].y, &normals[vi].z );
		else
			fscanf(infile, " %lf %lf %lf", &nx, &ny, &nz );
	}

	return;
}

void read_face_data( FILE * infile, FILE * colour_dump, face* faces, long int numfaces )
{
	long int fi = 0;
	int r, g, b, side = 0;

	for(; fi != numfaces; ++fi)
	{
		fscanf(infile, " %d", &faces[fi].sides);
		faces[fi].verts = malloc( faces[fi].sides * sizeof(long int) );

		for( side=0; side != faces[fi].sides; ++side )
			fscanf( infile, " %ld", &faces[fi].verts[side] );

		fscanf( infile, " %d %d %d", &r, &g, &b );
		fprintf( colour_dump, "%d %d %d\n", r, g, b );
	}

	return;
}

void write_off_file( FILE * outfile, vector * vertices, vector * normals, face * faces, long int numverts, long int numfaces
					, long int numedges, int has_normals )
{
	long int fi, vi = 0;
	int side;

	if( has_normals )
		fprintf( outfile, "NOFF\n");
	else
		fprintf( outfile, "OFF\n" );

	fprintf( outfile, "%ld %ld %ld\n", numverts, numfaces, numedges );

	/* write the vertex data */
	for(; vi != numverts; ++vi)
		fprintf( outfile, "%f %f %f\n", vertices[vi].x, vertices[vi].y, vertices[vi].z );

	for(fi=0; fi != numfaces; ++fi)
	{
		fprintf( outfile, "%d", faces[fi].sides );
		for(side = 0; side != faces[fi].sides; ++side)
			fprintf(outfile, " %ld", faces[fi].verts[side] );
		fprintf(outfile, "\n");
	}

	return;
}
