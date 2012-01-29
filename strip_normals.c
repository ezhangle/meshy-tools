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

	struct OFF mesh;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file> "
			"<where-to-dump-normals>\n", argv[0]);
		return 0;
	}

	infile	= fopen( argv[1], "r" );

	read_OFF_data(infile, &mesh);

	if(!mesh.has_normals)
	{
		fprintf(stderr, "Header does not begin with 'N' or 'CN'"
				" - is this a (C)NOFF file?\n");
		fclose(infile);
		exit(0);
	}

	fclose(infile);

	outfile = fopen( argv[2], "w");
	write_off_file(outfile
		, &mesh
		, 0
		, mesh.has_colours);
	fclose(outfile);

	if(argv[3] != NULL)
		normal_file = fopen(argv[3], "w");

	if(normal_file != NULL)
	{
		write_normal_file(normal_file
			, mesh.vert_normals
			, mesh.numverts);
		fclose(normal_file);
	}

	free_mesh(&mesh);

	return 0;
}

void write_normal_file( FILE *normal_file
	, vector *normals
	, long int numverts )
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
		fprintf(normal_file, "%f %f %f\n"
			, normals[vi].x, normals[vi].y, normals[vi].z );

	return;
}
