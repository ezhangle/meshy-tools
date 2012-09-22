#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utilities.h"

/*
*	This program swaps the coordinates of vertices in OFF files
*/
int main(int argc, char *argv[])
{
	FILE *mesh_fp = NULL;

	enum {	XY_SWAP, XZ_SWAP, YZ_SWAP };
	int to_swap = XY_SWAP;
	float tmp;

	struct OFF mesh = empty_mesh;
	unsigned int i = 0U;

	if(argc!=3)
	{
		printf("Syntax is: %s <infile>"
			" <\"xy\" swap>\n", argv[0]);
		return 0;
	}

	if(!strcmp("xy", argv[2]))
		to_swap = XY_SWAP;
	else if(!strcmp("xz", argv[2]))
		to_swap = XZ_SWAP;
	else if(!strcmp("yz", argv[2]))
		to_swap = YZ_SWAP;

	open_file(&mesh_fp, argv[1], "r+");

	read_OFF_data(mesh_fp, &mesh);
	
	rewind(mesh_fp);
	
	/* remember to swap the normals as well */
	switch(to_swap)
	{
	case XY_SWAP:
		for(i=0; i!=mesh.numverts; ++i)
		{
			tmp = mesh.vertices[i].x;
			mesh.vertices[i].x = mesh.vertices[i].y;
			mesh.vertices[i].y = tmp;
		}
		break;
	case XZ_SWAP:
		for(i=0; i!=mesh.numverts; ++i)
		{
			tmp = mesh.vertices[i].x;
			mesh.vertices[i].x = mesh.vertices[i].z;
			mesh.vertices[i].z = tmp;
		}
		break;

	case YZ_SWAP:
		for(i=0; i!=mesh.numverts; ++i)
		{
			tmp = mesh.vertices[i].y;
			mesh.vertices[i].y = mesh.vertices[i].z;
			mesh.vertices[i].z = tmp;
		}
		break;
	}

	write_off_file(mesh_fp, &mesh, mesh.has_normals, mesh.has_colours);
	fclose(mesh_fp);
	
	return EXIT_SUCCESS;
}
