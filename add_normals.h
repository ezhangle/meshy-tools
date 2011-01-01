#include "utilities.h"

typedef struct{ vector centre, norm; } fc_normal;

/* a vertex augmented with normal information and associated face data */
typedef struct{
	int assoc_faces;
	long int *faces;
} vert_extra;

void format_data( face *faces
	, vertex *vertices
	, vertex *std_verts
	, vector *std_norms
	, face *std_faces
	, long int numverts
	, long int numfaces);

void read_normal_file( FILE * normal_file
	, vector * vert_normals
	, long int numverts );

void calc_face_normals( face * faces
	, fc_normal *face_normals
	, vertex * vertices
	, long int numfaces);

void calc_face_barycentre( vertex * vertices
	, face * base
	, fc_normal * face_norm );

void find_face_associations( face * faces
	, vert_extra * vert_aug
	, long int numfaces );

void calc_vertex_normals( vertex * verts
	, vector *vert_normals
	, vert_extra *vert_aug
	, fc_normal *face_normals
	, long int numverts );

double vector_dist( vertex A, vector B);

void normalise_vector( vector * A );

