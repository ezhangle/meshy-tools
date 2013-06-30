#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

void set_colours(FILE *colourfile
	, struct OFF *mesh
	, char **argv
	, int stitch_colour );

int main(int argc, char **argv)
{
	FILE * infile = NULL;
	FILE * colourfile = NULL;
	FILE * outfile = NULL;

	int stitch_colour = 0;

	struct OFF mesh;

	initialise_mesh(&mesh);

	/* tell people about the syntax if they don't get it right */
	if( argc < 2 )
	{
		printf("Syntax is: %s <input> <output> <colour file>."
			"<input> can be the same as <output>, "
			"<colour file> iis optional."

			"\nTo use a uniform colour colour, the syntax is:\n"
			"%s <input-file> <output-file> <r> <g> <b>\n"
			"where 0.0 < r, g, b < 1.0\n", argv[0], argv[0]);

		return 0;
	}
	
	open_file(&infile, argv[1], "r");
	open_file(&outfile, argv[2], "w");

	if(argc == 3 && argv[3] != NULL)
	{
		stitch_colour = 1;
		open_file(&colourfile, argv[3], "r");
	}

	read_OFF_data(infile, &mesh);

	if(mesh.has_colours)
	{
		fprintf(stderr, "The file header claims that colour"
				" data are included, exiting.\n");
		fclose(infile);
		exit(EXIT_FAILURE);	
	}
	fclose(infile);

	set_colours(colourfile
		, &mesh
		, argv
		, stitch_colour );

	write_off_file(outfile
		, &mesh
		, mesh.has_normals
		, 1);

	fclose(outfile);
	fclose(colourfile);

	free_mesh(&mesh);

	return 0;
}

void set_colours( FILE * colourfile
	, struct OFF *mesh
	, char **argv
	, int stitch_colour )
{
	unsigned long int vi = 0;
	float new_r, new_g, new_b;

	colour *colours = mesh->colours;
	mesh->colours = calloc(mesh->numverts, sizeof(colour));

	if(!mesh->colours)
	{
		fprintf(stderr, "Unable to allocate memory for"
				" colours, aborting.\n");
		free_mesh(mesh);
		exit(EXIT_FAILURE);
	}

	new_r = (float)atof(argv[3]);
	new_g = (float)atof(argv[4]);
	new_b = (float)atof(argv[5]);

	if(stitch_colour)
	{
		for(; vi!=mesh->numverts; ++vi)
		{
			fscanf(colourfile, "%lf %lf %lf",
				&colours[vi].r, &colours[vi].g, &colours[vi].b);
		}
	}
	else
	{
		for(; vi!=mesh->numverts; ++vi)
		{
			colours[vi].r = new_r;
			colours[vi].g = new_g;
			colours[vi].b = new_b;
		}
	}

	/* we've now defined colours, so indicate this */
	mesh->has_colours = 1;

	return;
}
