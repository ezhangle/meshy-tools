#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "utilities.h"


struct OFF mesh = empty_mesh;

void terminate(int sig)
{
	fprintf(stderr, "abnormal termination, signal: %d (%s)\n", sig, strerror(sig));

	free_mesh(&mesh);
	exit(EXIT_FAILURE);
}

/**
	This program reads a normal-augmented OFF file and
	produces a normal-augmented point cloud from the
	vertices.
*/
int main(int argc, char **argv)
{
	FILE *off_file;
	FILE *xyz_file;

	unsigned long i = 0;

	signal(SIGINT, terminate);
	signal(SIGSEGV, terminate);
	signal(SIGTERM, terminate);

	if ( argc != 3 )
	{
		printf("Syntax is: %s <off-file> <xyz_file>\n", argv[0] );
		return 1;
	}

	open_file(&off_file, argv[1], "r");
	read_OFF_data(off_file, &mesh);
	fclose(off_file);


	open_file(&xyz_file, argv[2], "w");
	for (; i != mesh.numverts; ++i)
	{
		if ( 0 > fprintf(xyz_file, "%f %f %f\n",
						 mesh.vertices[i].x,
						 mesh.vertices[i].y,
						 mesh.vertices[i].z))
		{
			fprintf(stderr, "Error writing to %s\n", argv[2]);
			terminate(SIGTERM);
		}
		if (mesh.has_normals)
		{
			if (0 > fprintf(xyz_file, " %f %f %f",
							mesh.vert_normals[i].x,
							mesh.vert_normals[i].y,
							mesh.vert_normals[i].z))
			{
				fprintf(stderr, "Error writing to %s\n", argv[2]);
				terminate(SIGTERM);
			}
		}
		if (0 > fprintf(xyz_file, "\n"))
		{
			terminate(SIGTERM);
		}
	}

	fclose(off_file);
	fclose(xyz_file);

	return 0;
}
