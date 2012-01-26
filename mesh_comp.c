#include <stdio.h>

int main()
{
	FILE *fp_mesh_A = NULL;
	FILE *fp_mesh_B = NULL;

	struct OFF mesh_A;
	struct OFF mesh_B;

	open_file(&fp_mesh_A, "r");
	open_file(&fp_mesh_B, "r");

	initialise_mesh(&mesh_A);
	initialise_mesh(&mesh_B);

	read_OFF_data(fp_mesh_A, &mesh_A);
	read_OFF_data(fp_mesh_B, &mesh_B);

	analyse_mesh(&mesh_A, 0);
	analyse_mesh(&mesh_B, 1);

	return 0;
}


void analyse_mesh(struct OFF *mesh, unsigned short idx)
{
	unsigned long i=0UL;
	unsigned long j=0UL;

	for(; i!=mesh->numfaces; ++i)
	{
		for(j=0; j!=mesh->faces[i].sides; ++j)
		{
			v = mesh->faces[i].verts[j];
			n = nearest_node(mesh->vertices[v]);

			float dist = vector_distance(n, mesh->vertices[v]);
			vector norm = mesh->vert_normals[v];

			if(dist < nodes[n].sep[idx])
			{
				nodes[n].sep[idx] = dist;
				nodes[n].norm[idx] = norm;
			}
		}
	}

	return;
}
