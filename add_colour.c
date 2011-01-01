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
	unsigned long int numverts, numfaces, numedges;

	/* tell people about the syntax if they don't get it right */
	if( argc < 2 ){
		printf("Syntax is: %s <input-file> <output-file> <colour_file>\n", argv[0]);
		printf("<input-file> can be the same as <output-file>.\n");
		printf("If <colour_file> is provided, its colour data will be used\n");
		printf("\n");
		printf("To use a uniform colour colour, the syntax is:\n");
		printf("%s <input-file> <output-file> <r> <g> <b>\n", argv[0]);
		printf("where 0.0 < r, g, b < 1.0");

		return 0;
	}
	
	infile = fopen( argv[1], "r" );

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
		exit(1);	
	}

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	colours = malloc( numverts * sizeof(colour) );
	if( has_normals )
		normals = malloc( numverts * sizeof(vector) );

	/* if any memory allocation fails, just die */
	if( vertices == NULL || faces== NULL || (has_normals && normals == NULL) || colours == NULL )
	{
		fprintf(stderr, "Inital memory allocation failed.\n");
		exit(1);
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

	if( has_normals )
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

	new_r = atof( argv[3] );
	new_g = atof( argv[4] );
	new_b = atof( argv[5] );

	/*fprintf(stderr, "%f %f %f\n", new_r, new_g, new_b);*/

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
