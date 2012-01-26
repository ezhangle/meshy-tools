#include <stdio.h>
#include <stdlib.h>
#include "utilities.h"

void initialise_mesh(struct OFF *mesh)
{
	mesh->vertices = NULL;
	mesh->vert_normals = NULL;
	mesh->vert_aug = NULL;

	mesh->faces = NULL;
	mesh->face_normals = NULL;
	mesh->colours = NULL;

	mesh->has_colours = 0;
	mesh->has_normals = 0;
}

void allocate_mesh_memory(struct OFF *mesh)
{
	mesh->vertices = malloc(mesh->numverts
					* sizeof(*mesh->vertices));
	if(!mesh->vertices)
	{
		fprintf(stderr, "Unable to allocate vertex memory\n");
		exit(EXIT_FAILURE);
	}

	if(mesh->has_normals)
	{
		mesh->vert_normals = malloc(mesh->numverts
					* sizeof(*mesh->vert_normals));
		if(!mesh->vert_normals)
		{
			fprintf(stderr, "Unable to allocate memory for"
					" vertex normals.\n");
			free_mesh(mesh);
			exit(EXIT_FAILURE);
		}
	}

	if(mesh->has_colours)
	{
		mesh->colours = malloc(mesh->numverts
					* sizeof(*mesh->colours));
		if(!mesh->colours)
		{
			fprintf(stderr, "Unable to allocate memory for"
					" vertex colours.\n");
			free_mesh(mesh);
			exit(EXIT_FAILURE);
		}
	}

	mesh->faces = malloc(mesh->numfaces * sizeof(*mesh->faces));
	if(!mesh->faces)
	{
		fprintf(stderr, "Unable to allocate vertex memory\n");
		free_mesh(mesh);
		exit(EXIT_FAILURE);
	}


}

void free_mesh(struct OFF *mesh)
{
	free(mesh->vertices);
	free(mesh->vert_normals);
	free(mesh->vert_aug);

	free(mesh->faces);
	free(mesh->face_normals);

	free(mesh->colours);
	return;
}

void read_OFF_data(FILE *infile, struct OFF *mesh)
{
	read_off_header(infile, mesh);

	allocate_mesh_memory(mesh);

	read_vertex_data(infile, mesh);

	read_face_data(infile, mesh);

	return;
}

void open_file(FILE **fp
		, char *filename
		, char *mode)
{
	*fp = fopen(filename, mode);

	if(fp)
		return;

	fprintf(stderr, "Unable to open %s, aborting.\n", filename);
	exit(EXIT_FAILURE);
}

void read_off_header(FILE *infile, struct OFF *mesh)
{
	int ch = fgetc(infile);

	if('C' == ch)
	{
		ch = fgetc(infile);
		mesh->has_colours = 1;
	}

	if('N' == ch)
	{
		ch = fgetc(infile);
		mesh->has_normals = 1;
	}
	
	fgetc(infile);
	fgetc(infile);

	fscanf(infile, "%lu %lu %lu", &mesh->numverts
		, &mesh->numfaces
		, &mesh->numedges);

	return;
}

void read_vertex_data(FILE *infile, struct OFF *mesh)
{
	unsigned long vi = 0;

	for(; vi !=mesh->numverts; ++vi)
	{
		fscanf(infile, " %f %f %f", &mesh->vertices[vi].x
				, &mesh->vertices[vi].y
				, &mesh->vertices[vi].z );

		if(mesh->has_normals && mesh->vert_normals != NULL)
		{
			fscanf(infile, " %f %f %f", &mesh->vert_normals[vi].x
				, &mesh->vert_normals[vi].y
				, &mesh->vert_normals[vi].z );
		}
		else if(mesh->has_normals && mesh->vert_normals == NULL)
		{
			fscanf(infile, " %*f %*f %*f");
		}

		if(mesh->has_colours && mesh->colours != NULL)
		{
			fscanf( infile, " %f %f %f 1", &mesh->colours[vi].r
				, &mesh->colours[vi].g, &mesh->colours[vi].b );
		}
		else if(mesh->has_colours && mesh->colours == NULL)
		{
			fscanf(infile, " %*f %*f %*f 1");
		}
	}

	return;
}

void read_face_data(FILE * infile, struct OFF *mesh)
{
	unsigned long fi = 0;
	unsigned short side = 0;
	face *this_face = NULL;

	for(; fi != mesh->numfaces; ++fi)
	{
		fscanf(infile, "%hu ", &mesh->faces[fi].sides);
		this_face = &mesh->faces[fi];

		if(this_face->sides==0)
		{
			fprintf(stderr, "face %lu has 0 sides.\n", fi);
			this_face->verts = NULL;
			continue;
		}

		this_face->verts = malloc( this_face->sides 
					* sizeof(*(this_face->verts)));

		for(side=0; side != this_face->sides; ++side )
		{
			if( EOF == fscanf( infile, " %ld"
						, &this_face->verts[side] ) )
			{
				fprintf(stderr, "face data error, exiting\n");
				exit(EXIT_FAILURE);
			}
		}
	}

	return;
}

void write_off_file( FILE * outfile
		, vertex * vertices
		, vector * normals
		, face * faces
		, colour * colours
		, unsigned long numverts
		, unsigned long numfaces
		, unsigned long numedges
		, int write_normals
		, int write_colours )
{
	unsigned long fi = 0, vi = 0;
	unsigned short side;

	if(write_colours)
		fprintf(outfile, "C");

	if(write_normals)
		fprintf(outfile, "N");

	fprintf(outfile, "OFF\n");

	fprintf(outfile, "%lu %lu %lu\n", numverts, numfaces, numedges);

	/* write the vertex data */
	for(; vi != numverts; ++vi)
	{
		fprintf( outfile, "%f %f %f"
			, vertices[vi].x, vertices[vi].y, vertices[vi].z );

		if( write_normals )
			fprintf(outfile, " %f %f %f",
				normals[vi].x, normals[vi].y, normals[vi].z );

		if( write_colours )
			fprintf(outfile, " %f %f %f",
				colours[fi].r, colours[fi].g, colours[fi].b );

		fprintf(outfile, "\n");
	}

	for(; fi != numfaces; ++fi)
	{
		fprintf(outfile, "%hu", faces[fi].sides);
		if(faces[fi].sides == 0)
			fprintf(stderr, "fucked\n");
		for(side = 0; side != faces[fi].sides; ++side)
			fprintf(outfile, " %ld", faces[fi].verts[side] );

		fprintf(outfile, "\n");
	}

	return;
}
