#include <stdio.h>
#include "grid.h"

int write_vtu(
	GRID &g,						// a grid
	FE_VEC u[],				// fe vectors
	int n_vectors,		// no of fe vectors
	char* name				// filename to write to, will overwrite w/o warning
){

	int i,j; // counter
	FILE *fp;

	for(i = 0; i < n_vectors; i++) {
		if (g.num_nodes() != u[i].length()) {
			printf("Grid and fe vector do not match!\n");
			return -1;
		}
	}
 
	if ((fp = fopen(name, "w")) == NULL) {
		printf("Could not open %s for writing.\n", name);
		return -1;
	}
	
	// vtk-header
	fprintf(fp,"<?xml version=\"1.0\" ?>\n");
  	fprintf(fp,"<VTKFile type=\"UnstructuredGrid\" version=\"0.1\">\n");
 	fprintf(fp,"\t<UnstructuredGrid>\n");
        fprintf(fp,"\t\t<Piece NumberOfPoints=\"%d\" ",g.num_nodes());
	fprintf(fp,"NumberOfCells=\"%d\">\n",g.num_triangles());
	
	if (n_vectors > 0) {
		fprintf(fp,"\t\t\t<PointData Scalars=\"%s\">\n", u[0].getName());
		for(i = 0; i < n_vectors; i++) {
			// vtk-Pointdata
			fprintf(fp,"\t\t\t\t<DataArray Name=\"%s\" type=\"Float64\" format=\"ascii\">\n", u[i].getName());
                        for(j = 0; j < g.num_nodes(); j++){
				fprintf(fp,"\t\t\t\t\t%lf\n",u[i][j]);
			}
			fprintf(fp,"\t\t\t\t</DataArray>\n");
		}
		fprintf(fp,"\t\t\t</PointData>\n");
	} else {
		fprintf(fp,"\t\t\t<PointData />\n");
	}
	
	fprintf(fp,"\t\t\t<CellData />\n");
	
  	// vtk-Points = nodes
	fprintf(fp,"\t\t\t<Points>\n");
	// for paraview, everything has to be 3d, even if it is 2d
	fprintf(fp,"\t\t\t\t<DataArray type=\"Float64\" Name=\"Array\" NumberOfComponents=\"3\" format=\"ascii\">\n");
        for(i = 0; i < g.num_nodes(); i++){
          fprintf(fp,"\t\t\t\t\t%lf %lf %lf \n", g.get_coordinates(i)[0], g.get_coordinates(i)[1], 0.0);
	}
	fprintf(fp,"\t\t\t\t</DataArray>\n\t\t\t</Points>\n");

  	// vtk-Cells = triangs.
	fprintf(fp,"\t\t\t<Cells>\n");
	fprintf(fp,"\t\t\t\t<DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\">\n");
        for(i = 0; i < g.num_triangles(); i++){
		fprintf(fp,"\t\t\t\t\t%ld %ld %ld \n",
                        static_cast<long>(g.get_triangle(i)[0]), static_cast<long>(g.get_triangle(i)[1]), static_cast<long>(g.get_triangle(i)[2]));
	}
	fprintf(fp,"\t\t\t\t</DataArray>\n");
	fprintf(fp,"\t\t\t\t<DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\">\n");
        for(i = 0; i < g.num_triangles(); i++){
		fprintf(fp,"\t\t\t\t\t%ld \n", static_cast<long>((i+1)*NODES_PER_TRIANGLE));
	}
	fprintf(fp,"\t\t\t\t</DataArray>\n");
	fprintf(fp,"\t\t\t\t<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n");
        for(i = 0; i < g.num_triangles(); i++){
		fprintf(fp,"\t\t\t\t\t%d \n", 5);
	}
	fprintf(fp,"\t\t\t\t</DataArray>\n");
	fprintf(fp,"\t\t\t</Cells>\n");

  	// vtk tail
  	fprintf(fp,"\t\t</Piece>\n");
	fprintf(fp,"\t</UnstructuredGrid>\n");
	fprintf(fp,"</VTKFile>\n");

	fclose(fp);
	return 0;
}
