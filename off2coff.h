typedef struct{
	double x, y, z;
} vector;

typedef struct{
	int r,g,b;
} colour;

typedef struct{
	int sides;
	long int *verts;
} face;

void read_vertex_data(FILE * infile, vector * vertices, vector * normals, long int numverts, int has_normals );

void read_face_data( FILE * infile, face* faces, long int numfaces );

void write_coff_file( FILE * outfile, vector * vertices, vector * normals, face * faces, long int numverts, long int numfaces
					, long int numedges, int has_normals, colour paint);
