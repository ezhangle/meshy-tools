#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"

double distance(vector *A, vector *B);

void switch_orient(vector normals[]
	, long vi);

int consistent_orient(vertex vertices[]
	, vector normals[]
	, long authorities[]
	, long vert);

void free_faces(face *faces, unsigned long numfaces);

void switch_orient(vector normals[]
	, long vi)
{
	vector *norm = &normals[ vi ];

	norm->x = -norm->x;
	norm->y = -norm->y;
	norm->z = -norm->z;

	return;
}

int consistent_orient(vertex vertices[]
	, vector normals[]
	, long authorities[]
	, long vi)
{
	/* positions of ends of normals */
	vector pos_A, pos_B, neg_B;

	vertex *A = &vertices[ authorities[vi] ];
	vertex *B = &vertices[ vi ];

	vector *norm_A = &normals[ authorities[vi] ];
	vector *norm_B = &normals[ vi ];

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

int main(int argc, char *argv[])
{
	int has_colour = 0;
	int has_normals = 0;

	colour *colours = NULL;
	vertex *vertices = NULL;
	vector *normals = NULL;
	face *faces = NULL;

	/* authorities[vertex in question] = authority on its orientation */
	long *authorities = NULL;

	unsigned long numfaces;
	unsigned long numverts;
	unsigned long numedges;

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

	vertices = calloc(numverts, sizeof(*vertices));
	if(vertices==NULL)
	{
		fprintf(stderr, "Unable to allocate memory for vertices.\n");
		exit(EXIT_FAILURE);
	}

	authorities = calloc(numverts, sizeof(*authorities));
	if(authorities==NULL)
	{
		fprintf(stderr, "Unable to allocate memory for authorities.\n");
		exit(EXIT_FAILURE);
	}
	for(vi=0L; vi!=numverts; ++vi)
		authorities[vi] = (long)-1;

	normals = calloc(numverts, sizeof(*normals));
	if(normals==NULL)
	{
		free(vertices);
		fprintf(stderr, "Unable to allocate memory for vertices.\n");
		exit(EXIT_FAILURE);
	}

	faces = calloc(numfaces, sizeof(*faces));
	if(faces==NULL)
	{
		free(normals);
		free(vertices);
		fprintf(stderr, "Unable to allocate memory for faces.\n");
		exit(EXIT_FAILURE);
	}

	if(has_colour)
	{
		colours = calloc(numverts, sizeof(*colours));
		if(colours==NULL)
		{
			free(vertices);
			free(normals);
			free_faces(faces, numfaces);
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
		long int v = -1;

		if((fi%1000) == 1)
			fprintf(stderr, "fi = %lu\n", fi);
		
		v = faces[fi].verts[1];

		if(authorities[v] == -1L)
			authorities[v] = fi;

		v = faces[fi].verts[2];
		if(authorities[v] == -1L)
			authorities[v] = fi;
	}

	for(vi=0L; vi<numverts; ++vi)
	{
		if( !consistent_orient(vertices, normals, authorities, vi))
			switch_orient(normals, vi);
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

	free(vertices);
	free(normals);
	free_faces(faces, numfaces);
	free(authorities);
	free(colours);
	fclose(infile);
	fclose(outfile);
	
	return 0;
}

void free_faces(face *faces, unsigned long numfaces)
{
	unsigned long fi = 0;

	for(; fi!=numfaces; ++fi)
	{
		free(faces[fi].verts);
		faces[fi].verts = NULL;
	}
	free(faces);
	return;
}

