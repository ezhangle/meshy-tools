#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "off2noff.h"

/**	This program is off2noff, it calculates vertex normals from the faces and creates a NOFF file
	if the input is a COFF file, it generates a CNOFF file
*/

int main( int argc, char **argv )
{
	FILE * infile, *outfile, *normfile;
	vertex * vertices = NULL;
	face * faces = NULL;
	colour * colours = NULL;

	char c;
	int is_coloured = 0;
	int stitch_normals = 0;
	long int numverts, numfaces, numedges;

	/* tell people about the syntax if they don't get it right */
	if( argc == 0 ){
		printf("Syntax is: %s <input-file> <output-file> <normal_file>\n", argv[0]);
		printf("<input-file> can be the same as <output-file>.\n");
		printf("If <normal_file> is provided, its normal data will be used instead of calculating normals\n");
		return 0;
	}
	
	infile = fopen( argv[1], "r" );

	if( argv[3] != NULL )
		stitch_normals = 1;

	c = fgetc(infile);
	fgetc(infile);
	fgetc(infile);	

	/* if it's a COFF file then note this and take and extra character */
	if( c == 'C' )
	{
		fgetc(infile);
		is_coloured = 1;
	}
	else
	{
		is_coloured = 0;
	}
	
	fscanf(infile, " %ld", &numverts);	
	fscanf(infile, " %ld", &numfaces);		
	fscanf(infile, " %ld", &numedges);		
	
	/* make enough space for the data and calculated values */
	vertices =	calloc( numverts, sizeof(vertex) );
	faces =		calloc( numfaces, sizeof(face) ) ;

	if( vertices == NULL || faces == NULL )
	{
		fprintf(stderr, "inital memory allocation failed.\n");
		exit(1);
	}

	if( is_coloured )
	{
		colours = malloc( numfaces * sizeof(colour) );
		if( colours == NULL )
		{
			fprintf(stderr, "colour memory assignment failed.\n");
			exit(1);
		}
	}

	read_vertex_data( infile, vertices, numverts );
	read_face_data(	infile, faces, colours, numfaces, is_coloured );

	fclose(infile);

	if( stitch_normals == 0 ){
		calc_face_normals( faces, vertices, numfaces );
		find_face_associations( faces, vertices, numfaces, numverts );
		calc_vertex_normals( vertices, faces,  numfaces, numverts );
	}
	else
		normfile = fopen( argv[3], "r" );

	if( argc == 1 )
		outfile = fopen( argv[1], "w" );
	else
		outfile = fopen( argv[2], "w" );

	write_noff_file( outfile, normfile, numverts, numfaces, numedges, vertices, faces, colours, is_coloured, stitch_normals );

	printf("done.\n");
	return 0;
}

void read_vertex_data(FILE * infile, vertex* vertices, long int numverts)
{
	long int vi = 0;

	for(; vi != numverts; ++vi)
		fscanf(infile, "%lf %lf %lf", &vertices[vi].pos.x, &vertices[vi].pos.y, &vertices[vi].pos.z );

	return;
}

void read_face_data( FILE * infile, face* faces, colour * colours, long int numfaces, int is_coloured )
{
	long int side, fi = 0;
	for(; fi != numfaces; ++fi)
	{
		fscanf(infile, " %d", &faces[fi].sides);
		faces[fi].verts = malloc( faces[fi].sides * sizeof(long int) );

		for( side=0; side != faces[fi].sides; ++side )
			fscanf( infile, " %ld", &faces[fi].verts[side] );

		if( is_coloured )
			fscanf( infile, " %d %d %d", &colours[fi].r, &colours[fi].g, &colours[fi].b );
	}
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
void calc_face_normals( face * faces, vertex * vertices, long int numfaces )
{
	vector A, B;
	long int fi, first, second, third;

	for(fi = 0; fi != numfaces; ++fi)
	{
		first = faces[fi].verts[0];
		second = faces[fi].verts[1];
		third = faces[fi].verts[2];

		A.x = vertices[ second ].pos.x - vertices[ first ].pos.x ;
		A.y = vertices[ second ].pos.y - vertices[ first ].pos.y ;
		A.z = vertices[ second ].pos.z - vertices[ first ].pos.z ;

		B.x = vertices[ third ].pos.x - vertices[ first ].pos.x ;
		B.y = vertices[ third ].pos.y - vertices[ first ].pos.y ;
		B.z = vertices[ third ].pos.z - vertices[ first ].pos.z ;

		/* take the cross product of A and B: ORIENTATION UNDEFINED! */
		faces[fi].norm.x = (A.y * B.z) - (B.y * A.z);
		faces[fi].norm.y = (A.x * B.z) - (B.x * A.z);
		faces[fi].norm.z = (A.x * B.y) - (B.x * A.y);

		normalise_vector( &faces[fi].norm );
		calc_face_barycentre(vertices, &faces[fi] );
	}
	return;
}

void calc_face_barycentre( vertex * vertices, face * base )
{
	int i = 0;
	base->centre.x = base->centre.y = base->centre.z = 0;

	for(; i != base->sides; ++i)
	{
		base->centre.x += vertices[  base->verts[i]  ].pos.x;
		base->centre.y += vertices[  base->verts[i]  ].pos.y;
		base->centre.z += vertices[  base->verts[i]  ].pos.z;	
	}
	
	base->centre.x /= (double)base->sides;
	base->centre.y /= (double)base->sides;
	base->centre.z /= (double)base->sides;

	return;
}

/** find all faces that each vertex borders */
void find_face_associations(face * faces, vertex * vertices, long int numfaces, long int numverts )
{
	long int vi, fi = 0;
	int i=0;

	for( fi=0; fi != numfaces; ++fi)
	{
		for(i=0; i != faces[fi].sides; ++i)
		{
			vi = faces[fi].verts[i];
			++vertices[vi].assoc_faces;

			vertices[vi].faces = (long int*)realloc( vertices[vi].faces, vertices[vi].assoc_faces * sizeof(long int) );
			if( vertices[vi].faces == NULL )
			{
				fprintf(stderr, "memory allocation failed\n");
				exit(1);
			}

			vertices[vi].faces[ vertices[vi].assoc_faces -1 ] = fi;
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
	double dist = 0;

	xdiff *= xdiff;
	ydiff *= ydiff;
	zdiff *= zdiff;
	
	dist = sqrt( xdiff + ydiff + zdiff );
	return dist;
}


void calc_vertex_normals( vertex * vertices, face * faces, long int numfaces, long int numverts ){
	long int face_ind = 0;
	long int vi = 0;
	int afi;		/* associated faces index */
	double * barycentre_dist = NULL;

	for(; vi != numverts; ++vi)
	{
		if( vertices[vi].faces == NULL || vertices[vi].assoc_faces == 0 )
		{
			vertices[vi].norm.x = 1;
			continue;
		}

		barycentre_dist = (double *)malloc( vertices[vi].assoc_faces * sizeof(double) );

		if( barycentre_dist == NULL )
		{
			fprintf(stderr, "memory allocation failed\n");
			exit(1);
		}

		for( afi = 0; afi != vertices[vi].assoc_faces; ++afi)
		{
			face_ind = vertices[vi].faces[afi];
			barycentre_dist[afi] = vector_dist( vertices[vi].pos, faces[face_ind].centre );

			vertices[vi].norm.x += faces[face_ind].norm.x / ( 1 + barycentre_dist[afi] );
			vertices[vi].norm.y += faces[face_ind].norm.y / ( 1 + barycentre_dist[afi] );
			vertices[vi].norm.z += faces[face_ind].norm.z / ( 1 + barycentre_dist[afi] );
		}
		
		normalise_vector( &vertices[vi].norm );
	}
	return;	
}

/** Actually write out the NOFF file */
void write_noff_file( FILE * outfile, FILE * normfile, long int numverts, long int numfaces, long int numedges
					, vertex * vertices, face * faces, colour * colours, int is_coloured, int stitch_normals )
{
	long int i = 0;
	int side;
	vector norm;

	if( is_coloured )
		fprintf( outfile, "CNOFF\n" );
	else
		fprintf( outfile, "NOFF\n" );

	fprintf( outfile, "%ld %ld %ld\n", numverts, numfaces, numedges );

	for( i = 0; i != numverts; ++i){
		fprintf( outfile, "%f %f %f ", vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z );

		if( stitch_normals == 0 )
			fprintf( outfile, "%f %f %f\n", vertices[i].norm.x, vertices[i].norm.y, vertices[i].norm.z );
		else{
			fscanf(normfile, " %lf %lf %lf", &norm.x, &norm.y, &norm.z );
			fprintf( outfile, " %f %f %f\n", norm.x, norm.y, norm.z );
		}
	}

	for( i=0; i != numfaces; ++i)
	{
		fprintf( outfile, "%d", faces[i].sides );

		for( side = 0; side != faces[i].sides; ++side )
			fprintf( outfile, " %ld", faces[i].verts[side] );		

		if( is_coloured )
			fprintf( outfile, " %d %d %d", colours[i].r, colours[i].g, colours[i].b );		
			
		fprintf( outfile, "\n" );
	}
	return;
}
