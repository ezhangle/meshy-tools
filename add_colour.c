#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

void set_colours( FILE * colourfile
	, char **argv
	, colour * colours
	, unsigned long int numverts
	, int *has_colour
	, int stitch_colour );

int main(int argc, char **argv)
{
	FILE * infile;
	FILE * colourfile;
	FILE * outfile;

	vertex * vertices = NULL;
	vector * normals = NULL;
	face * faces = NULL;
	colour * colours = NULL;

	int has_colour = 0;
	int stitch_colour = 0;
	int has_normals = 0;

	unsigned long numverts = 0;
	unsigned long numfaces = 0;
	unsigned long numedges = 0;

	/* tell people about the syntax if they don't get it right */
	if( argc < 2 )
	{
		printf("Syntax is: %s <input> <output> <colour file>."
			"<input> can be the same as <output>, "
			"<colour file> iis optional."

			"\nTo use a uniform colour colour, the syntax is:\n"
			"%s <input-file> <output-file> <r> <g> <b>\n"
			"where 0.0 < r, g, b < 1.0", argv[0], argv[0]);

		return 0;
	}
	
	infile = fopen( argv[1], "r" );

	if(!infile)
	{
		fprintf(stderr, "Error opening %s, aborting.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	if( argv[3] != NULL )
		stitch_colour = 1;

	read_off_header(infile
		, &has_normals
		, &has_colour
		, &numverts
		, &numfaces
		, &numedges);

	if( has_colour )
	{
		fprintf(stderr, "The file header claims that colour data are included, exiting.\n");
		fclose(infile);
		exit(EXIT_FAILURE);	
	}

	vertices = calloc(numverts, sizeof(vertex));
	faces = calloc(numfaces, sizeof(face));
	colours = calloc(numverts, sizeof(colour));
	if( has_normals )
		normals = calloc(numverts, sizeof(vector));

	/* if any memory allocation fails, just die */
	if( vertices == NULL || faces== NULL
		|| (has_normals && normals == NULL) || colours == NULL )
	{
		fprintf(stderr, "Inital memory allocation failed.\n");
		exit(EXIT_FAILURE);
	}
	if( stitch_colour )
		colourfile = fopen( argv[3], "r" );

	read_vertex_data( infile
		, vertices
		, normals
		, colours
		, numverts
		, has_normals
		, 0);

	read_face_data(	infile
		, faces
		, numfaces);

	fclose(infile);

	set_colours(colourfile
		, argv
		, colours
		, numverts
		, &has_colour
		, stitch_colour );

	outfile = fopen( argv[2], "w" );
	write_off_file(outfile
		, vertices
		, normals
		, faces
		, colours
		, numverts
		, numfaces
		, numedges
		, has_normals
		, has_colour );

	fclose(outfile);

	free(vertices);
	free(faces);
	free(colours);
	free(normals);

	return 0;
}

void set_colours( FILE * colourfile
	, char **argv
	, colour *colours
	, unsigned long int numverts
	, int *has_colour
	, int stitch_colour )
{
	unsigned long int vi = 0;
	float new_r, new_g, new_b;

	new_r = (float)atof(argv[3]);
	new_g = (float)atof(argv[4]);
	new_b = (float)atof(argv[5]);

	if( stitch_colour )
		for(; vi!=numverts; ++vi)
		{
			colours[vi].r = new_r;
			colours[vi].g = new_g;
			colours[vi].b = new_b;
		}
	else
		for(; vi!=numverts; ++vi)
			fscanf(colourfile, "%f %f %f", &colours[vi].r
				, &colours[vi].g, &colours[vi].b);

	/* we've now defined colours, so indicate this */
	*has_colour = 1;

	return;
}
