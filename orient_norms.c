#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"

double distance(vector *A, vector *B);

void switch_orient(vector normals[]
	, long vi);

int consistent_orient(vector vertices[]
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

int consistent_orient(vector vertices[]
	, vector normals[]
	, long authorities[]
	, long vi)
{
	/* positions of ends of normals */
	vector pos_A, pos_B, neg_B;

	vector *A = &vertices[ authorities[vi] ];
	vector *B = &vertices[ vi ];

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

	/* authorities[vertex in question] = authority on its orientation */
	long *authorities = NULL;

	unsigned long numpairs = 0;
	unsigned long fi = 0;
	unsigned long vi = 0;

	FILE *infile = NULL;
	FILE *outfile = NULL;

	face *faces;
	vector *vertices;

	struct OFF mesh;
	initialise_mesh(&mesh);

	if(argc != 3)
	{
		fprintf(stderr, "syntax is: %s <infile> <outfile>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	open_file(&infile, argv[1], "r");
	open_file(&outfile, argv[2], "w");

	read_OFF_data(infile, &mesh);

	if(!mesh.has_normals)
	{
		fprintf(infile, "Normals must be present to orient them!\n");
		free_mesh(&mesh);
		exit(EXIT_FAILURE);
	}

	faces = mesh.faces;
	vertices = mesh.vertices;

	authorities = calloc(mesh.numverts, sizeof(*authorities));
	if(authorities==NULL)
	{
		fprintf(stderr, "Unable to allocate memory for authorities.\n");
		exit(EXIT_FAILURE);
	}
	for(vi=0L; vi!=mesh.numverts; ++vi)
		authorities[vi] = (long)-1;

	for(; fi<mesh.numfaces; ++fi)
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

	for(vi=0L; vi<mesh.numverts; ++vi)
	{
		if( !consistent_orient(vertices
				, mesh.vert_normals
				, authorities, vi))
		{
			switch_orient(mesh.vert_normals, vi);
		}
	}

	write_off_file(outfile
		, &mesh
		, mesh.has_normals
		, mesh.has_colours);

	free_mesh(&mesh);
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

