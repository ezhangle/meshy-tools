#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "add_normals.h"

/*
*	This program calculates vertex normals from the faces and creates a
*	NOFF file if the input is a COFF file, it generates a CNOFF file
*/

int main( int argc, char **argv )
{
	FILE *infile;
	FILE *outfile;
	FILE *normfile;

	struct OFF mesh;

	int stitch_normals = 0;

	initialise_mesh(&mesh);

	/* tell people about the syntax if they don't get it right */
	if ( argc != 2 && argc != 3)
	{
		printf(	"Syntax is: %s <input-file> <output-file> "
				"<normal_file>\n", argv[0]);
		printf(	"<input-file> can be the same as <output-file>.\n");
		printf(	"If <normal_file> is provided, its normal data "
				"will be used instead of calculating normals\n");
		return 0;
	}

	open_file(&infile, argv[1], "r");
	open_file(&outfile, argv[2], "w");

	if (argv[3] != NULL)
	{
		open_file(&normfile, argv[3], "r");
	}

	read_OFF_data(infile, &mesh);
	fclose(infile);

	if (mesh.has_normals)
	{
		fprintf(stderr, "The file header claims that normals"
				" are included, exiting.\n");
		free_mesh(&mesh);
		fclose(infile);
		exit(EXIT_FAILURE);
	}

	/* argv[3] is the name of the file that contains normals */
	if (argv[3] != NULL)
	{
		stitch_normals = 1;
	}
	else
	{
		mesh.vert_aug = malloc(mesh.numverts
							   * sizeof(*mesh.vert_aug));
		mesh.face_normals = malloc(mesh.numfaces
								   * sizeof(*mesh.face_normals));
	}

	if (!mesh.vert_aug || !mesh.face_normals)
	{
		fprintf(stderr, "Unable to allocate necessary memory\n");
		free_mesh(&mesh);
		exit(EXIT_FAILURE);
	}


	fprintf(stderr, "read all data\n");
	if (stitch_normals == 0)
	{
		calc_face_normals(&mesh);
		find_face_associations(&mesh);
		calc_vertex_normals(&mesh);

		free(mesh.face_normals);
		mesh.face_normals = NULL;
	}
	else
	{
		read_normal_file(normfile
						 , mesh.vert_normals
						 , mesh.numverts);
		fclose(normfile);
	}

	write_off_file(outfile
				   , &mesh
				   , 1
				   , mesh.has_colours);
	fclose(outfile);

	printf("done.\n");

	free_mesh(&mesh);

	return 0;
}

/* read normal data */
void read_normal_file( FILE *normal_file
					   , vector *vert_normals
					   , long int numverts )
{
	long int vi = 0;
	for (; vi != numverts; ++vi)
	{
		fscanf(normal_file, "%lf %lf %lf"
			   , &vert_normals[vi].x
			   , &vert_normals[vi].y
			   , &vert_normals[vi].z );
	}

	return;
}

/**	calculate a face normal by taking the cross product of two of its sides
*	the orientation is not considered (are faces' vertex lists consistently
*	clockwise?)
*/
void calc_face_normals(struct OFF *mesh)
{
	vector A, B;
	unsigned long fi, first, second, third;

	face *faces = mesh->faces;
	fc_normal *face_normals = mesh->face_normals;
	vector *vertices = mesh->vertices;

	for (fi = 0; fi != mesh->numfaces; ++fi)
	{
		first = faces[fi].verts[0];
		second = faces[fi].verts[1];
		third = faces[fi].verts[2];

		A.x = vertices[ second ].x - vertices[ first ].x ;
		A.y = vertices[ second ].y - vertices[ first ].y ;
		A.z = vertices[ second ].z - vertices[ first ].z ;

		B.x = vertices[ third ].x - vertices[ first ].x ;
		B.y = vertices[ third ].y - vertices[ first ].y ;
		B.z = vertices[ third ].z - vertices[ first ].z ;

		/* take the cross product of A and B: ORIENTATION UNDEFINED! */
		face_normals[fi].norm.x = (A.y * B.z) - (B.y * A.z);
		face_normals[fi].norm.y = (A.x * B.z) - (B.x * A.z);
		face_normals[fi].norm.z = (A.x * B.y) - (B.x * A.y);

		normalise_vector( &face_normals[fi].norm );
		calc_face_barycentre(vertices, &faces[fi], &face_normals[fi] );
	}
	return;
}

void calc_face_barycentre( vector *vertices
						   , face *base
						   , fc_normal *face_norm )
{
	int i = 0;
	face_norm->centre.x = face_norm->centre.y = face_norm->centre.z = 0;

	for (; i != base->sides; ++i)
	{
		face_norm->centre.x += vertices[  base->verts[i]  ].x;
		face_norm->centre.y += vertices[  base->verts[i]  ].y;
		face_norm->centre.z += vertices[  base->verts[i]  ].z;
	}

	face_norm->centre.x /= (double)base->sides;
	face_norm->centre.y /= (double)base->sides;
	face_norm->centre.z /= (double)base->sides;

	return;
}

/** find all facesthat each vertex borders */
void find_face_associations(struct OFF *mesh)
{

	face *faces = mesh->faces;
	vert_extra *vert_aug = mesh->vert_aug;

	unsigned long vi, fi, size_needed;
	int i = 0;

	for ( fi = 0; fi != mesh->numfaces; ++fi)
	{
		for (i = 0; i != faces[fi].sides; ++i)
		{
			vi = faces[fi].verts[i];
			vert_aug[vi].assoc_faces++;
			size_needed = vert_aug[vi].assoc_faces * sizeof(long);

			vert_aug[vi].faces = realloc( vert_aug[vi].faces, size_needed );
			if ( vert_aug[vi].faces == NULL )
			{
				fprintf(stderr, "memory allocation failed\n");
				exit(1);
			}

			vert_aug[vi].faces[ vert_aug[vi].assoc_faces - 1 ] = fi;
		}
	}
	return;
}

/** find the Euclidean (straight line) distance between two points */
double vector_dist( vector A, vector B)
{
	double xdiff = A.x - B.x;
	double ydiff = A.y - B.y;
	double zdiff = A.z - B.z;

	xdiff *= xdiff;
	ydiff *= ydiff;
	zdiff *= zdiff;

	return sqrt( xdiff + ydiff + zdiff );
}


void calc_vertex_normals(struct OFF *mesh)
{
	long int face_ind = 0;
	unsigned long vi = 0;
	int afi;		/* associated facesindex */
	double *barycentre_dist = NULL;

	vector *vertices = mesh->vertices;
	vector *vert_normals = mesh->vert_normals;
	vert_extra *vert_aug = mesh->vert_aug;
	fc_normal *face_normals = mesh->face_normals;

	for (; vi != mesh->numverts; ++vi)
	{
		if ( vert_aug[vi].faces == NULL || vert_aug[vi].assoc_faces == 0 )
		{
			vert_normals[vi].x = 1;
			continue;
		}

		barycentre_dist = malloc( vert_aug[vi].assoc_faces * sizeof(double) );

		if ( barycentre_dist == NULL )
		{
			fprintf(stderr, "memory allocation failed\n");
			exit(1);
		}

		for ( afi = 0; afi != vert_aug[vi].assoc_faces; ++afi)
		{
			face_ind = vert_aug[vi].faces[afi];
			barycentre_dist[afi] = vector_dist( vertices[vi], face_normals[face_ind].centre );

			vert_normals[vi].x += face_normals[face_ind].norm.x / ( 1 + barycentre_dist[afi] );
			vert_normals[vi].y += face_normals[face_ind].norm.y / ( 1 + barycentre_dist[afi] );
			vert_normals[vi].z += face_normals[face_ind].norm.z / ( 1 + barycentre_dist[afi] );
		}

		normalise_vector( &vert_normals[vi] );
	}

	free(barycentre_dist);
	return;
}
