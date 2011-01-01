#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"

typedef struct {
	long authority;
	long testee;
} test_pair;

double distance(vector *A, vector *B);

void switch_orient(vector normals[], test_pair pair);

int add_test_pair(test_pair **pair_list
	, unsigned long *numpairs
	, test_pair *new_pair);

int consistent_orient(vertex vertices[]
	, vector normals[]
	, test_pair pair);

int needs_testing(test_pair *already_done
	, unsigned long numpairs
	, long vi);

void switch_orient(vector normals[], test_pair pair)
{
	vector *norm = &normals[pair.testee];

	norm->x = -norm->x;
	norm->y = -norm->y;
	norm->z = -norm->z;

	return;
}

int consistent_orient(vertex vertices[], vector normals[], test_pair pair)
{
	/* positions of ends of normals */
	vector pos_A, pos_B, neg_B;

	vertex *A = &vertices[pair.authority];
	vertex *B = &vertices[pair.testee];

	vector *norm_A = &normals[pair.authority];
	vector *norm_B = &normals[pair.testee];

	pos_A.x = A->x + norm_A->x;
	pos_A.y = A->y + norm_A->y;
	pos_A.z = A->z + norm_A->z;

	pos_B.x = B->x + norm_B->x;
	pos_B.y = B->y + norm_B->y;
	pos_B.z = B->z + norm_B->z;

	neg_B.x = B->x - norm_B->x;
	neg_B.y = B->y - norm_B->y;
	neg_B.z = B->z - norm_B->z;

	if( distance(&pos_A, &pos_B) < distance(&pos_A, &neg_B) )
	{
		return 1;
	}

	return 0;
}

double distance(vector *A, vector *B)
{
	double dx, dy, dz;

	dx = A->x - B->x;
	dy = A->y - B->y;
	dz = A->z - B->z;

	return sqrt(dx*dx + dy*dy + dz*dz);
}

int needs_testing(test_pair *already_done
	, unsigned long numpairs
	, long vi)
{
	unsigned long i = 0;
	for(; i<numpairs; ++i)
	{
		if(already_done[i].authority == vi)
			return 0;

		if(already_done[i].testee == vi)
			return 0;
	}
	return 1;
}

int add_test_pair(test_pair **pair_list
	, unsigned long *numpairs
	, test_pair *new_pair)
{
	test_pair *tmp = NULL;

	tmp = realloc( *pair_list, (*numpairs +1) * sizeof(test_pair));
	if(tmp==NULL)
	{
		fprintf(stderr, "Unable to allocate memory.\n");
		return 0;
	}
	*pair_list = tmp;

	(*pair_list)[*numpairs] = *new_pair;

	++*numpairs;

	return 1;
}

int main(int argc, char *argv[])
{
	int has_colour = 0;
	int has_normals = 0;

	colour *colours = NULL;
	vertex *vertices = NULL;
	vector *normals = NULL;
	face *faces = NULL;

	unsigned long numfaces;
	unsigned long numverts;
	unsigned long numedges;

	test_pair *vert_list = NULL;
	test_pair tmp;

	unsigned long numpairs = 0;
	unsigned long fi = 0;
	unsigned long vi = 0;

	FILE *infile = NULL;
	FILE *outfile = NULL;

	if(argc != 3)
	{
		fprintf(stderr, "syntax is: %s <infile> <outfile>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	infile = fopen(argv[1], "r");
	if(infile == NULL)
	{
		fprintf(stderr, "Unable to open %s.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

 	outfile = fopen(argv[2], "w");
	if(outfile == NULL)
	{
		fprintf(stderr, "Unable to open %s.\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	read_off_header(infile
		, &has_normals
		, &has_colour
		, &numverts
		, &numfaces
		, &numedges);

	if( !has_normals )
	{
		fprintf(infile, "Normals must be present to orient them!\n");
		exit(EXIT_FAILURE);
	}

	vertices = (vertex*)malloc(numverts * sizeof(*vertices));
	if(vertices==NULL)
	{
		fprintf(stderr, "Unable to allocate memory for vertices.\n");
		exit(EXIT_FAILURE);
	}

	normals = (vector*)malloc(numverts * sizeof(*normals));
	if(normals==NULL)
	{
		free(vertices);
		fprintf(stderr, "Unable to allocate memory for vertices.\n");
		exit(EXIT_FAILURE);
	}

	faces = (face*)malloc(numfaces * sizeof(*faces));
	if(faces==NULL)
	{
		free(normals);
		free(vertices);
		fprintf(stderr, "Unable to allocate memory for faces.\n");
		exit(EXIT_FAILURE);
	}

	if(has_colour)
	{
		colours = (colour*)malloc(numverts * sizeof(*colours));
		if(colours==NULL)
		{
			free(vertices);
			free(normals);
			free(faces);
			fprintf(stderr, "No memory for colours\n");
			exit(EXIT_FAILURE);
		}
	}

	read_vertex_data(infile
		, vertices
		, normals
		, colours
		, numverts
		, has_normals
		, has_colour);
	read_face_data(infile, faces, numfaces);

	for(; fi<numfaces; ++fi)
	{
		if(fi%1000 == 0)
			fprintf(stderr, "fi = %lu\n", fi);

		if(needs_testing(vert_list, numpairs, faces[fi].verts[1]))
		{
			tmp.authority = faces[fi].verts[0];
			tmp.testee = faces[fi].verts[1];

			if(0==add_test_pair(&vert_list, &numpairs, &tmp))
			{
				free(vert_list);
				free(vertices);
				free(normals);
				free(faces);
				free(colours);
				fclose(infile);
				fclose(outfile);
				exit(EXIT_FAILURE);
			}
		}

		if(needs_testing(vert_list, numpairs, faces[fi].verts[2]))
		{
			tmp.authority = faces[fi].verts[0];
			tmp.testee = faces[fi].verts[2];

			if(0==add_test_pair(&vert_list, &numpairs, &tmp))
			{
				free(vert_list);
				free(vertices);
				free(normals);
				free(faces);
				free(colours);
				fclose(infile);
				fclose(outfile);
				exit(EXIT_FAILURE);
			}
		}
	}

	for(; vi<numpairs; ++vi)
	{
		if( !consistent_orient(vertices, normals, vert_list[vi]) )
			switch_orient(normals, vert_list[vi]);
	}

	write_off_file(outfile
		, vertices
		, normals
		, faces
		, colours
		, numverts
		, numfaces
		, numedges
		, has_normals
		, has_colour);


	free(vert_list);
	free(vertices);
	free(normals);
	free(faces);
	free(colours);
	fclose(infile);
	fclose(outfile);
	
	return 0;
}
