#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "noff2off.h"

/* This program is noff2off, it changes a NOFF file to an OFF file, discarding the normal data */

int main( int argc, char **argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * normfile;

	vector * vertices = NULL;
	long int numverts, numfaces, numedges;
	int is_coloured = 0;

	if( argc < 3 ){
		printf("Syntax is: %s <input-file> <output-file> <where-to-dump-normals>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );
	normfile= fopen( argv[3], "w" );

	/* if it's a CNOFF file, take an extra character */
	if( 'C' == fgetc(infile) )
	{
		is_coloured = 1;
		fgetc(infile);
	}

	fgetc(infile);
	fgetc(infile);
	fgetc(infile);

	fscanf(infile, " %ld", &numverts);	
	fscanf(infile, " %ld", &numfaces);		
	fscanf(infile, " %ld", &numedges);		

	vertices = (vector*)malloc( numverts * sizeof(vector) );

	/* get vertex data and close files now in case infile = outfile */
	read_vertex_data( infile, normfile, vertices, numverts );
	fclose(infile);
	fclose(normfile);

	outfile	= fopen( argv[2], "w" );
	write_noff_file(outfile, infile, vertices, numverts, numfaces, numedges, is_coloured);
	fclose(outfile);

	return 0;
}

void read_vertex_data(FILE * infile, FILE * normfile, vector* vertices, long int numverts)
{
	long int vi = 0;
	double nx, ny, nz;

	for(; vi != numverts; ++vi)
	{
		fscanf(infile, " %lf %lf %lf", &vertices[vi].x, &vertices[vi].y, &vertices[vi].z );
		fscanf(infile, " %lf %lf %lf", &nx, &ny, &nz );
		fprintf(normfile, "%f %f %f\n", nx, ny, nz );
	}

	return;
}

void write_noff_file( FILE * outfile, FILE * infile, vector * vertices, long int numverts, long int numfaces, long int numedges, int is_coloured )
{
	long int vi = 0;
	int buffer;

	if( is_coloured )
		fprintf(outfile, "COFF\n");
	else
		fprintf( outfile, "OFF\n" );

	fprintf( outfile, "%ld %ld %ld\n", numverts, numfaces, numedges );

	/* write the vertex data */
	for(; vi != numverts; ++vi)
		fprintf( outfile, "%f %f %f\n", vertices[vi].x, vertices[vi].y, vertices[vi].z );

	/* blindy copy the rest of the file: preserving face colours if present */
	while( (buffer = fgetc(infile)) != EOF )
		fputc( buffer , outfile );	

	return;
}

