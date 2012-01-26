#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

int main(int argc, char *argv[])
{
	FILE *fp_mesh_A = NULL;
	FILE *fp_mesh_B = NULL;

	struct OFF mesh_A;
	struct OFF mesh_B;

	if(argc != 2)
	{
		printf("Syntax is: %s <mesh A> <mesh B>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	open_file(&fp_mesh_A, argv[1], "r");
	open_file(&fp_mesh_B, argv[2], "r");

	initialise_mesh(&mesh_A);
	initialise_mesh(&mesh_B);

	read_OFF_data(fp_mesh_A, &mesh_A);
	read_OFF_data(fp_mesh_B, &mesh_B);

	analyse_mesh(&mesh_A, 0);
	analyse_mesh(&mesh_B, 1);

	free_mesh(&mesh_A);
	free_mesh(&mesh_B);

	return 0;
}


void analyse_mesh(struct OFF *mesh, unsigned short idx)
{
	unsigned long i=0UL;
	unsigned long j=0UL;

	unsigned long v;
	

	for(; i!=mesh->numfaces; ++i)
	{
		for(j=0; j!=mesh->faces[i].sides; ++j)
		{
			v = mesh->faces[i].verts[j];
			(void)idx;
/*
			n = nearest_node(mesh->vertices[v]);

			float dist = vector_distance(n, mesh->vertices[v]);
			vector norm = mesh->vert_normals[v];

			if(dist < nodes[n].sep[idx])
			{
				nodes[n].sep[idx] = dist;
				nodes[n].norm[idx] = norm;
			}
*/
		}
	}

	return;
}
