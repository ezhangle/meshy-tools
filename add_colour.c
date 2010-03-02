#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

void set_face_data( FILE * colourfile, char **argv, colour * colours, unsigned long int numfaces, int *new_r, int *new_g, int *new_b, int *has_colour, int stitch_colour );

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
	int new_r, new_g, new_b;
	unsigned long int numverts, numfaces, numedges;

	/* tell people about the syntax if they don't get it right */
	if( argc < 2 ){
		printf("Syntax is: %s <input-file> <output-file> <colour_file>\n", argv[0]);
		printf("<input-file> can be the same as <output-file>.\n");
		printf("If <colour_file> is provided, its colour data will be used\n");
		printf("\n");
		printf("To give all faces the same colour, use the following syntax:\n");
		printf("%s <input-file> <output-file> <r> <g> <b>\n", argv[0]);
		printf("Where <r>, <g> and <b> are integers in the range 0-255\n");

		return 0;
	}
	
	infile = fopen( argv[1], "r" );

	if( argv[3] != NULL )
		stitch_colour = 1;

	read_off_header(infile, &has_normals, &has_colour, &numverts, &numfaces, &numedges);

	if( has_colour )
	{
		fprintf(stderr, "The file header claims that colour data are included, exiting.\n");
		fclose(infile);
		exit(1);	
	}

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	colours = malloc( numfaces * sizeof(colour) );
	if( has_normals )
		normals = malloc( numfaces* sizeof(vector) );

	/* if any memory allocation fails, just die */
	if( vertices == NULL || faces== NULL || (has_normals && normals == NULL) || colours == NULL )
	{
		fprintf(stderr, "Inital memory allocation failed.\n");
		exit(1);
	}
	if( stitch_colour )
		colourfile = fopen( argv[3], "r" );

	read_vertex_data( infile, vertices, normals, numverts, has_normals );
	read_face_data(	infile, faces, colours, numfaces, has_colour );
	fclose(infile);

	set_face_data( colourfile, argv, colours, numfaces, &new_r, &new_g, &new_b, &has_colour, stitch_colour );

	outfile = fopen( argv[2], "w" );
	write_off_file(outfile, vertices, normals, faces, colours, numverts, numfaces, numedges, has_normals, has_colour );

	fclose(outfile);

	free(vertices);
	free(faces);
	free(colours);

	if( has_normals )
		free(normals);

	return 0;
}

void set_face_data( FILE * colourfile, char **argv, colour * colours, unsigned long int numfaces, int *new_r, int *new_g, int *new_b, int *has_colour, int stitch_colour )
{
	unsigned long int fi = 0;

	*new_r = atoi( argv[3] );
	*new_g = atoi( argv[4] );
	*new_b = atoi( argv[5] );

	if( stitch_colour )
		for(; fi!= numfaces; ++fi)
		{
			colours[fi].r = *new_r;
			colours[fi].g = *new_g;
			colours[fi].b = *new_b;
		}
	else
		for(; fi!= numfaces; ++fi)
			fscanf(colourfile, "%d %d %d", &colours[fi].r, &colours[fi].g, &colours[fi].b);

	/* we've now defined colours, so indicate this */
	*has_colour = 1;

	return;
}
