#include "utilities.h"

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

void calc_face_normals(struct OFF *mesh);

void calc_face_barycentre( vertex * vertices
	, face * base
	, fc_normal * face_norm );

void find_face_associations(struct OFF *mesh);

void calc_vertex_normals(struct OFF *mesh);

double vector_dist( vertex A, vector B);

void normalise_vector( vector * A );

