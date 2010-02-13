typedef struct{
	float x, y, z;
} vector;

typedef struct{
	int r, g, b;
} colour;

typedef struct{
	int sides;
	long int *verts;
} face;


void read_vertex_data(FILE * infile, vector* vertices, vector * normals, long int numverts, int has_normals );


void read_face_data( FILE * infile, face* faces, colour * colours, long int numfaces, int has_colour );


void write_off_file( FILE * outfile, vector * vertices, vector * normals, face * faces, colour * colours, long int numverts, long int numfaces, long int numedges, int has_normals, int has_colour );
