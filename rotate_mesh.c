#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "utilities.h"

int main(int argc, char *argv[])
{
	FILE *input = NULL;
	FILE *output = NULL;
	struct OFF mesh;
	double angle = 0.0;
	char *input_name = NULL;
	char *output_name = NULL;

	vector cv = { 0.0, 0.0, 0.0 };

	unsigned long i = 0UL;

	double cosangle = 0.0;
	double sinangle = 0.0;

	if(argc != 4)
	{
		fprintf(stderr, "syntax is: %s: <input> <output> <angle>\n"
			, argv[0]);
		return 1;
	}

	initialise_mesh(&mesh);
	input_name = argv[1];
	output_name = argv[2];
	angle = atof(argv[3]);

	cosangle = cos(angle);
	sinangle = sin(angle);

	/* read input OFF file */
	open_file(&input, input_name, "r");
	read_OFF_data(input, &mesh);
	fclose(input);


	/* perform the rotation */
	for(; i!=mesh.numverts; ++i)
	{
		cv = mesh.vertices[i];

		mesh.vertices[i].y = (cosangle*cv.y) - (sinangle*cv.z);
		mesh.vertices[i].z = (sinangle*cv.y) + (cosangle*cv.z);
	}



	/* write rotated OFF file */
	open_file(&output, output_name, "w");
	write_off_file(output, &mesh, 0, 0);
	fclose(output);

	free_mesh(&mesh);

	return 0;
}
