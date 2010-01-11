typedef struct{
	double x, y, z;
} vector;

typedef struct{
	vector pos;
	vector norm;

	long int assoc_faces;
	long int * faces;
} vertex;

typedef struct{
	int r,g,b;
} colour;

typedef struct{
	int sides;
	long int *verts;

	vector centre;
	vector norm;
} face;

void read_vertex_data(FILE * infile, vertex* vertices, long int numverts);

void read_face_data( FILE * infile, face* faces, colour * colours, long int numfaces, int is_coloured );

void calc_face_normals( face * faces, vertex * vertices, long int numfaces );

void calc_face_barycentre( vertex * vertices, face * base );

void find_face_associations(face * faces, vertex * vertices, long int numfaces, long int numverts );

void calc_vertex_normals( vertex * vertices, face * faces, long int numfaces, long int numverts );

void write_noff_file( FILE * outfile, long int numverts, long int numfaces
			, long int numedges, vertex * vertices, face * faces, colour * colours, int is_coloured );

double vector_dist( vector A, vector B);

void normalise_vector( vector * A );

