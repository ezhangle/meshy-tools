#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "utilities.h"

/** this program centres a mesh (OFF format)
*	on the origin
*/
int main(int argc, char **argv)
{
	struct OFF mesh = empty_mesh;
	unsigned int i = 0U, k=0U;
	int basename_len = 0;

	/* extremal vertex coordinates in the mesh*/
	double axis_max[3], axis_min[3];
	vector axis[3];

	char mesh_name_aa[200];
	char *mesh_evec_name = NULL;

	char *mesh_name = NULL;
	FILE *mesh_fp = NULL;
	FILE *evec_fp = NULL;
	FILE *aa_fp = NULL;

	if(argc != 3)
	{
		printf("Syntax is: %s <mesh> <evec_name>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	mesh_name = argv[1];
	open_file(&mesh_fp, mesh_name, "r");
	read_OFF_data(mesh_fp, &mesh);
	fclose(mesh_fp);

	/* work out names for later */
	mesh_evec_name = argv[2];
	basename_len = strlen(mesh_evec_name) - strlen(".evec");	/* remove '.evec' */
	_snprintf(mesh_name_aa, sizeof(mesh_name_aa), "%.*s.aa", basename_len, mesh_evec_name);

	/* read eigenvector file */
	open_file(&evec_fp, mesh_evec_name, "r");

	for(k=0; k!=3; ++k)
	{
		fscanf(evec_fp, "%*f %lf %lf %lf", &axis[k].x, &axis[k].y, &axis[k].z);
		normalise_vector(&axis[k]);
		axis_max[k] = axis_min[k] = dot_product(&mesh.vertices[0], &axis[k]);
	}
	fclose(evec_fp);

	/* compute max/min along each axis */
	for(i=0; i!=mesh.numverts; ++i)
	{
		for(k=0; k!=3; ++k)
		{
			axis_max[k] = max(dot_product(&mesh.vertices[i], &axis[k]), axis_max[k]);
			axis_min[k] = min(dot_product(&mesh.vertices[i], &axis[k]), axis_min[k]);
		}
	}

	open_file(&aa_fp, mesh_name_aa, "w");
	for(k=0; k!=3; ++k)
		fprintf(aa_fp, "0 0 %f %f\n", axis_max[k], axis_min[k]);
	fclose(aa_fp);

	return EXIT_SUCCESS;
}
