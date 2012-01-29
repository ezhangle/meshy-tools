#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void write_colour_file(FILE *colour_file
	, struct OFF *mesh);

int main( int argc, char ** argv )
{
	FILE * infile = NULL;
	FILE * outfile = NULL;
	FILE * colour_dump = NULL;

	struct OFF mesh;

	if( argc != 3 )
	{
		printf("Syntax is: %s <input-file> <output-file>"
			" <where-to-dump-colours>\n", argv[0]);
		return 0;
	}

	open_file(&infile, argv[1], "r");
	open_file(&outfile, argv[2], "w");
	open_file(&colour_dump, argv[3], "w");

	read_OFF_data(infile, &mesh);

	/* if it claims to have no colour data, exit */
	if(!mesh.has_colours)
	{
		fprintf(stderr, "Header does not begin with 'C'"
				" - is this a C(N)OFF file?\n");
		fclose(infile);
		free_mesh(&mesh);
		exit(0);
	}
	fclose(infile);


	/* !has_colour because we don't want to print the colour data here */
	write_off_file(outfile
		, &mesh
		, mesh.has_normals
		, 0);
	fclose(outfile);

	if(mesh.has_colours)
	{
		write_colour_file(colour_dump, &mesh);
		fclose(colour_dump);
	}

	free_mesh(&mesh);

	return 0;
}

void write_colour_file(FILE *colour_file
	, struct OFF *mesh)
{
	unsigned long vi = 0;
	colour *colours = mesh->colours;

	for(; vi !=mesh->numverts; ++vi)
	{
		fprintf(colour_file, "%f %f %f\n"
			, colours[vi].r, colours[vi].g, colours[vi].b);
	}

	return;
}
