#include <stdio.h>
#include <stdlib.h>
#include "off2coff.h"

int main( int argc, char ** argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * colourfile;

	vector * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;

	long int numverts, numfaces, numedges;
	int has_normals = 0;
	int stitch_colours = 0;
	colour paint;

	if( argc < 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> <red> <green> <blue>\n", argv[0]);
		printf("alternatively: %s <input-file> <output-file> <colour-file>\n", argv[0]);
		printf("<red>, <green> and <blue> are integers in the range 0-255.\n");
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	if( argc == 5 ){
		paint.r = atoi(argv[3]);
		paint.g = atoi(argv[4]);
		paint.b = atoi(argv[5]);
	}
	else{
		stitch_colours = 1;
		colourfile = fopen( argv[3], "r" );
	}

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
	read_face_data(infile, faces, numfaces);
	fclose(infile);

	outfile = fopen( argv[2], "w");
	write_coff_file(outfile, colourfile, vertices, normals, faces, numverts, numfaces, numedges, has_normals, stitch_colours, paint);
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

void read_face_data( FILE * infile, face* faces, long int numfaces )
{
	long int fi = 0;
	int side = 0;

	for(; fi != numfaces; ++fi)
	{
		fscanf(infile, " %d", &faces[fi].sides);
		faces[fi].verts = malloc( faces[fi].sides * sizeof(long int) );

		for( side=0; side != faces[fi].sides; ++side )
			fscanf( infile, " %ld", &faces[fi].verts[side] );
	}

	return;
}

void write_coff_file( FILE * outfile, FILE * colourfile, vector * vertices, vector * normals, face * faces, long int numverts, long int numfaces
					, long int numedges, int has_normals, int stitch_colours, colour paint)
{
	long int fi, vi = 0;
	int side;

	if( has_normals )
		fprintf( outfile, "CNOFF\n");
	else
		fprintf( outfile, "COFF\n" );

	fprintf( outfile, "%ld %ld %ld\n", numverts, numfaces, numedges );

	/* write the vertex data */
	for(; vi != numverts; ++vi)
		fprintf( outfile, "%f %f %f\n", vertices[vi].x, vertices[vi].y, vertices[vi].z );

	for(fi=0; fi != numfaces; ++fi)
	{
		fprintf( outfile, "%d ", faces[fi].sides );
		for(side = 0; side != faces[fi].sides; ++side)
			fprintf(outfile, "%ld ", faces[fi].verts[side] );

		if( stitch_colours ){
			fscanf( colourfile, " %d %d %d", &paint.r, &paint.g, &paint.b );
			fprintf(outfile, "%d %d %d\n", paint.r, paint.g, paint.b);
		}
		else
			fprintf(outfile, "%d %d %d\n", paint.r, paint.g, paint.b);
	}

	return;
}
