#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"
#define PI 3.14159265359

int main(int argc, char *argv[])
{
	FILE *input = NULL;
	FILE *output = NULL;
	FILE *angles = NULL;

	struct OFF mesh;
	char *input_name = NULL;
	char *output_name = NULL;
	char *angle_name = NULL;

	/* transformation matrix */
	double m[3][3];
	vector evecs[3];
	vector cv = { 0.0, 0.0, 0.0 };


	unsigned long i = 0UL;


	if(argc != 4)
	{
		fprintf(stderr, "syntax is: %s: <input> <output> <angle>\n"
			, argv[0]);
		return 1;
	}

	initialise_mesh(&mesh);
	input_name = argv[1];
	output_name = argv[2];
	angle_name = argv[3];


	/* read input OFF file */
	open_file(&input, input_name, "r");
	read_OFF_data(input, &mesh);
	fclose(input);

	
	open_file(&angles, angle_name, "r");
	fscanf(angles, "%*f %lf %lf %lf\n", &evecs[0].x, &evecs[0].y, &evecs[0].z);
	fscanf(angles, "%*f %lf %lf %lf\n", &evecs[1].x, &evecs[1].y, &evecs[1].z);
	fscanf(angles, "%*f %lf %lf %lf\n", &evecs[2].x, &evecs[2].y, &evecs[2].z);
	for(; i!=3; ++i)
	{
		m[i][0] = evecs[i].x;
		m[i][1] = evecs[i].y;
		m[i][2] = evecs[i].z;
	}
	fclose(angles);


	/* perform the rotation */
	for(i=0UL; i!=mesh.numverts; ++i)
	{
		cv = mesh.vertices[i];

		mesh.vertices[i].x =      (m[0][0]*cv.x)
					+ (m[0][1]*cv.y)
					+ (m[0][2]*cv.z);

		mesh.vertices[i].y =      (m[1][0]*cv.x)
					+ (m[1][1]*cv.y)
					+ (m[1][2]*cv.z);

		mesh.vertices[i].z =      (m[2][0]*cv.x)
					+ (m[2][1]*cv.y)
					+ (m[2][2]*cv.z);
	}

	/* write rotated OFF file */
	open_file(&output, output_name, "w");
	write_off_file(output, &mesh, 0, 0);
	fclose(output);

	free_mesh(&mesh);

	return 0;
}
