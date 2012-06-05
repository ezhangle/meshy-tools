#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "utilities.h"

int main(int argc, char **argv)
{
	struct OFF mesh = empty_mesh;

	int i = 0;
	double scale = 1.0;
	double factor = 1.0;

	char *mesh_name = NULL;
	FILE *mesh_fp = NULL;

	double max_x, max_y, max_z;
	double min_x, min_y, min_z;

#ifdef WINDOWS
	mesh_name = argv[1];
	scale = atof(argv[2]);

	(void)argc;

	printf("enter the desired x extension: ");
	if(1 != scanf("%f", &scale))
	{
		printf("\nno valid value received\n");
		exit(EXIT_FAILURE);
	}
	printf("\n");
#else
	if(argc != 3)
	{
		printf("Syntax is: %s <mesh> <x extension>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	mesh_name = argv[1];
	scale = atof(argv[2]);
#endif

	open_file(&mesh_fp, mesh_name, "r+");

	read_OFF_data(mesh_fp, &mesh);

	rewind(mesh_fp);

	/* initialise the min/max counters */
	max_x = mesh.vertices[0].x;
	max_y = mesh.vertices[0].y;
	max_z = mesh.vertices[0].z;

	min_x = mesh.vertices[0].x;
	min_y = mesh.vertices[0].y;
	min_z = mesh.vertices[0].z;

	for(i=0; i!=mesh.numverts; ++i)
	{
		max_x = max(mesh.vertices[i].x, max_x);
		max_y = max(mesh.vertices[i].y, max_y);
		max_z = max(mesh.vertices[i].z, max_z);

		min_x = min(mesh.vertices[i].x, min_x);
		min_y = min(mesh.vertices[i].y, min_y);
		min_z = min(mesh.vertices[i].z, min_z);
	}

	factor = scale / (max_x - min_x);
	for(i=0; i!=mesh.numverts; ++i)
	{
		mesh.vertices[i].x *= factor;
		mesh.vertices[i].y *= factor;
		mesh.vertices[i].z *= factor;
	}

	write_off_file(mesh_fp, &mesh, mesh.has_normals, mesh.has_colours);
	fclose(mesh_fp);

	return EXIT_SUCCESS;
}
