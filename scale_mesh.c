#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "utilities.h"
#include "macros.h"

/** this program centres a mesh (OFF format)
*	on the origin
*/
int main(int argc, char **argv)
{
	struct OFF mesh = empty_mesh;

	double scale = 1.0;

#ifdef WINDOWS
	mesh_name = "../../mesh.off";

	(void)argc;
	(void)argv;

	printf("enter a scale factor: ");
	if(1 != scanf("%f", &scale);
	{
		printf("\nno valid value received\n");
		exit(EXIT_FAILURE);
	}
	printf("\n");
#else
	if(argc != 3)
	{
		printf("Syntax is: %s <mesh> <scale>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	mesh_name = argv[1];
	scale = atof(argv[2]);
#endif

	open_file(&mesh_fp, mesh_name, "r+");

	read_OFF_data(mesh_fp, &mesh);
	
	rewind(mesh_fp);
	
	for(i=0; i!=mesh.numverts; ++i)
	{
		mesh.vertices[i].x *= scale;
		mesh.vertices[i].y *= scale;
		mesh.vertices[i].z *= scale;
	}

	write_off_file(mesh_fp, &mesh);
	fclose(mesh_fp);
	
	return EXIT_SUCCESS;
}
