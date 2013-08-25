#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"
#define PI 3.14159265359

int main(int argc, char *argv[])
{
	FILE *input = NULL;
	FILE *output = NULL;
	struct OFF mesh;
	double angle = 0.0;
	char *input_name = NULL;
	char *output_name = NULL;

	vector cv = { 0.0, 0.0, 0.0 };

	enum { X_axis, Y_axis, Z_axis };

	unsigned long i = 0UL;
	int axis = X_axis;

	double cosangle = 0.0;
	double sinangle = 0.0;

	if(argc != 5)
	{
		fprintf(stderr, "syntax is: %s: <input> <output> <x/y/z> <angle>\n"
			, argv[0]);
		return 1;
	}

	initialise_mesh(&mesh);
	input_name = argv[1];
	output_name = argv[2];
	axis = argv[3][0] - 'x';
	angle = atof(argv[4]);

	if(axis != X_axis && axis != Y_axis && axis != Z_axis)
	{
		fprintf(stderr, "axis (%d) must be 'x', 'y' or 'z'\n", axis);
		return 1;
	}

	cosangle = cos((angle * PI) / 180.0);
	sinangle = sin((angle * PI) / 180.0);

	/* read input OFF file */
	open_file(&input, input_name, "r");
	read_OFF_data(input, &mesh);
	fclose(input);


	/* perform the rotation */
	switch(axis)
	{
		case X_axis:
			for(; i!=mesh.numverts; ++i)
			{
				cv = mesh.vertices[i];

				mesh.vertices[i].y = (cosangle*cv.y) - (sinangle*cv.z);
				mesh.vertices[i].z = (sinangle*cv.y) + (cosangle*cv.z);
			}
			break;
		case Y_axis:
			for(; i!=mesh.numverts; ++i)
			{
				cv = mesh.vertices[i];

				mesh.vertices[i].x = (cosangle*cv.x) - (sinangle*cv.z);
				mesh.vertices[i].z = (sinangle*cv.x) + (cosangle*cv.z);
			}
			break;
		case Z_axis:
			for(; i!=mesh.numverts; ++i)
			{
				cv = mesh.vertices[i];

				mesh.vertices[i].x = (cosangle*cv.x) - (sinangle*cv.y);
				mesh.vertices[i].y = (sinangle*cv.x) + (cosangle*cv.y);
			}
			break;
	}



	/* write rotated OFF file */
	open_file(&output, output_name, "w");
	write_off_file(output, &mesh, 0, 0);
	fclose(output);

	free_mesh(&mesh);

	return 0;
}
