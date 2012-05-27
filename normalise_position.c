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

	/* extremal vertex coordinates in the mesh*/
	double max_x, max_y, max_z;
	double min_x, min_y, min_z;

#ifdef WINDOWS
	mesh_name = "../../mesh.off";

	(void)argc;
	(void)argv;
#else
	if(argc != 2)
	{
		printf("Syntax is: %s <mesh A>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	mesh_name = argv[1];
#endif

	open_file(&mesh_fp, mesh_name, "r");

	create_output_name(mesh_name, mesh_outname);

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
	
	double mid_x = (max_x + min_x)/2.0;
	double mid_y = (max_y + min_y)/2.0;
	double mid_z = (max_z + min_z)/2.0;
	
	for(i=0; i!=mesh.numverts; ++i)
	{
		mesh.vertices[i].x -= mid_x;
		mesh.vertices[i].y -= mid_y;
		mesh.vertices[i].z -= mid_z;

		mesh.vertices[i].x -= mid_x;
		mesh.vertices[i].y -= mid_y;
		mesh.vertices[i].z -= mid_z;
	}
	
	write_off_file(mesh_fp, &mesh);
	fclose(mesh_fp);
	
	return EXIT_SUCCESS;
}
