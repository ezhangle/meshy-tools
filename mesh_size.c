#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "utilities.h"

/** this program centres a mesh (OFF format)
*	on the origin
*/
int main(int argc, char **argv)
{
	struct OFF mesh = empty_mesh;
	int i = 0;

	/* extremal vertex coordinates in the mesh*/
	double max_x, max_y, max_z;
	double min_x, min_y, min_z;
	double mid_x, mid_y, mid_z;

	char *mesh_name = NULL;
	FILE *mesh_fp = NULL;

#ifdef WINDOWS
	mesh_name = argv[1];

	(void)argc;
#else
	if(argc != 2)
	{
		printf("Syntax is: %s <mesh A>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	mesh_name = argv[1];
#endif

	open_file(&mesh_fp, mesh_name, "r");

	read_OFF_data(mesh_fp, &mesh);
	
	fclose(mesh_fp);
	
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
	
	printf("x min = %f\n", min_x);
	printf("y min = %f\n", min_y);
	printf("z min = %f\n\n", min_z);

	printf("x max = %f\n", max_x);
	printf("y max = %f\n", max_y);
	printf("z max = %f\n\n", max_z);

	printf("x range = %f\n", max_x - min_x);
	printf("y range = %f\n", max_y - min_y);
	printf("z range = %f\n", max_z - min_z);

	return EXIT_SUCCESS;
}
