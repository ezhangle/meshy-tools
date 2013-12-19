typedef struct { double x, y, z; } vector;
typedef struct { double r, g, b; } colour;

#ifndef WINDOWS
	#define min(X,Y)	(((X) < (Y)) ? (X) : (Y))
	#define max(X,Y)	(((X) > (Y)) ? (X) : (Y))
#endif

typedef struct{ vector centre, norm; } fc_normal;

typedef struct{
	unsigned short sides;
	long int *verts;
} face;

#define empty_mesh { NULL, NULL, NULL,		NULL, NULL, NULL,		0, 0, 0, 0, 0 }

/* a vertex augmented with normal information and associated face data */
typedef struct{
	int assoc_faces;
	long int *faces;
} vert_extra;

struct OFF
{
	vector *vertices;
	vector *vert_normals;
	vert_extra *vert_aug;

	face *faces;
	fc_normal *face_normals;

	colour *colours;

	int has_colours;
	int has_normals;

	unsigned long numverts;
	unsigned long numfaces;
	unsigned long numedges;
};

void read_OFF_data(FILE *infile, struct OFF *mesh);

void analyse_mesh(struct OFF *mesh, unsigned short idx);

void initialise_mesh(struct OFF *mesh);

void free_mesh(struct OFF *mesh);

void open_file(FILE **fp
		, char *filename
		, char *mode);

void read_off_header(FILE * infile, struct OFF *mesh);

void read_vertex_data(FILE * infile, struct OFF *mesh);

void read_face_data( FILE * infile, struct OFF *mesh);

void write_off_file( FILE * outfile
	, struct OFF *mesh
	, int has_normals
	, int has_colour );

void rotate_vector(double rot[3][3]
	, vector *new_vector);

void setup_for_rotation(double rot[3][3]
	, int Axis
	, double theta);


double angle( vector A
	, vector B );

vector cross_product(vector A
	, vector B);

int evec_comp(const void *one
	, const void *two);

enum
{
	X_Axis,
	Y_Axis,
	Z_Axis
};

