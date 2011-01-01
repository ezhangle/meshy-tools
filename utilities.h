typedef struct { float x, y, z; } vector;

typedef struct { float x, y, z; } vertex;

typedef struct { float r, g, b; } colour;

typedef struct{
	int sides;
	long int *verts;
} face;


void read_vertex_data(FILE * infile
	, vertex *vertices
	, vector *normals
	, colour *colours
	, long int numverts
	, int has_normals
	, int has_colours );


void read_face_data( FILE * infile
	, face* faces
	, long int numfaces );


void write_off_file( FILE * outfile
	, vertex * vertices
	, vector * normals
	, face * faces
	, colour * colours
	, long int numverts
	, long int numfaces
	, long int numedges
	, int has_normals
	, int has_colour );


void read_off_header(FILE * infile
	, int * has_normals
	, int * has_colour
	, unsigned long int *numverts
	, unsigned long int *numfaces
	, unsigned long int *numedges);
