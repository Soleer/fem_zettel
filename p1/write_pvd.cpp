#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "grid.h"

#define DELIMITER '/'

// Extract the basename from a file
const char *basename(const char *filename)
{
	const char *base;
	if (filename == NULL)
	{
		return NULL;
	}
	else
	{
		base = strrchr(filename, DELIMITER);
		if (base != NULL)
			base++;
		return base;
	}
}

int write_pvd(
	GRID &g,	   // grid
	FE_VEC u[],	   // fe vectors
	int n_vectors, // no of fe vectors
	char *prefix,  // prefix for filenames
	int timestep,  // number of timestep
	double time	   // current time
)
{
	// Create filename
	FILE *fp;
	int pvdfilename_len, filename_len;
	char *pvdfilename;
	char *filename;
	pvdfilename_len = (int)strlen(prefix) + 5;
	pvdfilename = (char *)calloc(pvdfilename_len, sizeof(char));

	filename_len = strlen(prefix) + 7 + floor(log((double)timestep) / log(10.0));
	if (timestep == 0)
		filename_len = strlen(prefix) + 7;

	filename = (char *)calloc(filename_len, sizeof(char));

	sprintf(filename, "%s_%d.vtu", prefix, timestep);
	sprintf(pvdfilename, "%s.pvd", prefix);

	// First call
	if (timestep == 0)
	{
		if ((fp = fopen(pvdfilename, "w")) == NULL)
		{
			printf("Could not open %s for writing.\n", pvdfilename);
			return -1;
		}
		fprintf(fp, "<?xml version=\"1.0\"?>\n");
		fprintf(fp, "<VTKFile type=\"Collection\" version=\"0.1\">\n");
		fprintf(fp, "<Collection>\n");
		fprintf(fp, "<DataSet timestep=\"%e\" group=\"\" part=\"0\" file=\"%s\"/>\n",
				time, basename(filename));
		fprintf(fp, "</Collection>\n");
		fprintf(fp, "</VTKFile>\n");
	}
	else
	{
		if ((fp = fopen(pvdfilename, "r+")) == NULL)
		{
			printf("Could not open %s for writing.\n", pvdfilename);
			return -1;
		}
		fseek(fp, -25L, SEEK_END);
		fprintf(fp, "<DataSet timestep=\"%e\" group=\"\" part=\"0\" file=\"%s\"/>\n",
				time, basename(filename));
		fprintf(fp, "</Collection>\n");
		fprintf(fp, "</VTKFile>\n");
	}
	fclose(fp);

	return write_vtu(g, u, n_vectors, filename);
}
