struct OFF
{
	vertex *vertices;
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
