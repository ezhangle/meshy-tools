typedef struct{
	double x, y, z;
} vector;

void read_vertex_data(FILE * infile, FILE * normfile, vector* vertices, long int numverts);

void write_noff_file( FILE * outfile, FILE * infile, vector * vertices, long int numverts, long int numfaces, long int numedges, int is_coloured );

