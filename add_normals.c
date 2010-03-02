#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "add_normals.h"

/**	This program is off2noff, it calculates vertex normals from the facesand creates a NOFF file
	if the input is a COFF file, it generates a CNOFF file
*/

int main( int argc, char **argv )
{
	FILE * infile;
	FILE * outfile;
	FILE * normfile;

	vertex * vertices = NULL;
	vector * vert_normals = NULL;
	vert_extra * vert_aug = NULL;
	face * faces = NULL;
	fc_normal * face_normals = NULL;
	colour * colours = NULL;

	int has_colour = 0;
	int stitch_normals = 0;
	int has_normals = 0;
	unsigned long int numverts, numfaces, numedges;

	/* tell people about the syntax if they don't get it right */
	if( argc < 2 ){
		printf("Syntax is: %s <input-file> <output-file> <normal_file>\n", argv[0]);
		printf("<input-file> can be the same as <output-file>.\n");
		printf("If <normal_file> is provided, its normal data will be used instead of calculating normals\n");
		return 0;
	}
	
	infile = fopen( argv[1], "r" );

	if( argv[3] != NULL )
		stitch_normals = 1;

	read_off_header(infile, &has_normals, &has_colour, &numverts, &numfaces, &numedges);

	if( has_normals )
	{
		fprintf(stderr, "The file header claims that normals are included, exiting.\n");
		fclose(infile);
		exit(1);	
	}

	vertices = malloc( numverts * sizeof(vertex) );
	faces = malloc( numfaces * sizeof(face) );
	if( has_colour )
		colours = malloc( numfaces* sizeof(colour) );

	/* if any memory allocation fails, just die */
	if( vertices == NULL || faces== NULL || (has_colour && colours == NULL) )
	{
		fprintf(stderr, "inital memory allocation failed.\n");
		exit(1);
	}

	read_vertex_data( infile, vertices, vert_normals, numverts, has_normals );
	read_face_data(	infile, faces, colours, numfaces, has_colour );
	fclose(infile);

	if( stitch_normals == 0 )
	{
		calc_face_normals( faces, face_normals, vertices, numfaces);
		find_face_associations( faces, vertices, vert_aug, numfaces, numverts );
		calc_vertex_normals( vertices, vert_normals, vert_aug, faces, face_normals, numfaces, numverts );
	}
	else
	{
		normfile = fopen( argv[3], "r" );
		read_normal_file(normfile, vert_normals, numverts);
		fclose(normfile);
	}

	outfile = fopen( argv[2], "w" );
	write_off_file( outfile, vertices, vert_normals, faces, colours, numverts, numfaces, numedges, !has_normals, has_colour );
	fclose(outfile);

	printf("done.\n");
	return 0;
}

/* read normal data */
void read_normal_file( FILE * normal_file, vector * vert_normals, long int numverts )
{
	long int vi=0;
	for(; vi!=numverts; ++vi)
		fscanf(normal_file, "%f %f %f", &vert_normals[vi].x, &vert_normals[vi].y, &vert_normals[vi].z );

	return;
}

void normalise_vector( vector* A )
{
	double size = 0;
	double xx, yy, zz;

	/* square the components */
	xx = A->x * A->x;
	yy = A->y * A->y;
	zz = A->z * A->z;

	/* if the size is too small, assume something went wrong */
	size = sqrt( xx + yy + zz );

	A->x /= size;
	A->y /= size;
	A->z /= size;
	
	return;
}

/** calculate a face normal by taking the cross product of two of its sides
the orientation is not considered (are faces' vertex lists consistently clockwise?)
*/
void calc_face_normals( face * faces, fc_normal *face_normals,  vertex * vertices, long int numfaces)
{
	vector A, B;
	long int fi, first, second, third;

	for(fi = 0; fi != numfaces; ++fi)
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

void calc_face_barycentre( vertex * vertices, face * base, fc_normal * face_norm )
{
	int i = 0;
	face_norm->centre.x = face_norm->centre.y = face_norm->centre.z = 0;

	for(; i != base->sides; ++i)
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
void find_face_associations( face * faces, vertex * vertices, vert_extra * vert_aug, long int numfaces, long int numverts )
{
	long int vi, fi, size_needed;
	int i=0;

	for( fi=0; fi != numfaces; ++fi)
	{
		for(i=0; i != faces[fi].sides; ++i)
		{
			vi = faces[fi].verts[i];
			vert_aug[vi].assoc_faces++;
			size_needed = vert_aug[vi].assoc_faces * sizeof(long int);

			vert_aug[vi].faces = realloc( vert_aug[vi].faces, size_needed );
			if( vert_aug[vi].faces== NULL )
			{
				fprintf(stderr, "memory allocation failed\n");
				exit(1);
			}

			vert_aug[vi].faces[ vert_aug[vi].assoc_faces-1 ] = fi;
		}		
	}
	return;
}

/** find the Euclidean (straight line) distance between two points */
double vector_dist( vertex A, vector B)
{
	double xdiff = A.x - B.x;
	double ydiff = A.y - B.y;
	double zdiff = A.z - B.z;
	double dist = 0;

	xdiff *= xdiff;
	ydiff *= ydiff;
	zdiff *= zdiff;
	
	dist = sqrt( xdiff + ydiff + zdiff );
	return dist;
}


void calc_vertex_normals( vertex * vertices, vector *vert_normals, vert_extra * vert_aug
		, face * faces, fc_normal *face_normals, long int numfaces, long int numverts )
{
	long int face_ind = 0;
	long int vi = 0;
	int afi;		/* associated facesindex */
	double * barycentre_dist = NULL;

	for(; vi != numverts; ++vi)
	{
		if( vert_aug[vi].faces == NULL || vert_aug[vi].assoc_faces == 0 )
		{
			vert_normals[vi].x = 1;
			continue;
		}

		barycentre_dist = malloc( vert_aug[vi].assoc_faces * sizeof(double) );

		if( barycentre_dist == NULL )
		{
			fprintf(stderr, "memory allocation failed\n");
			exit(1);
		}

		for( afi = 0; afi != vert_aug[vi].assoc_faces; ++afi)
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
