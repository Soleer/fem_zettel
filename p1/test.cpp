#include <stdlib.h>
#include <stdio.h>
#include "sys/time.h"
#include <iostream>
#include <assert.h>

#include "grid.h"
#include "FE_VEC.h"

#include "config.h"

int main(int argc, char *argv[])
{

	timeval prgstart, prgende, solstart, solende;
	double start_s, end_s;
	GRID *g[grids];

	for (int i = 0; i < grids; ++i)
	{
		g[i] = new GRID;
		assert(g[i] != NULL);
	}

	gettimeofday(&prgstart, NULL);

	g[0]->read_from_file(coord_filename, conn_filename);

	for (int i = 1; i < grids; i++)
	{
		gettimeofday(&solstart, NULL);

		g[i - 1]->refine_ip(NULL, 0, *g[i], NULL);

		gettimeofday(&solende, NULL);

		start_s = solstart.tv_sec + solstart.tv_usec * 1.0e-6;
		end_s = solende.tv_sec + solende.tv_usec * 1.0e-6;
		std::cout << std::endl
				  << "The refinement from level " << i - 1 << " to level " << i << " took " << end_s - start_s << " seconds." << std::endl;

		std::cout << "Number of nodes on level " << i << ": " << g[i]->num_nodes() << std::endl;
		std::cout << "Number of triangles on level " << i << ": " << g[i]->num_triangles() << std::endl;
	}

	gettimeofday(&prgende, NULL);
	start_s = prgstart.tv_sec + prgstart.tv_usec * 1.0e-6;
	end_s = prgende.tv_sec + prgende.tv_usec * 1.0e-6;
	std::cout << std::endl
			  << "The refinement took " << end_s - start_s << " seconds" << std::endl;

	for (int i = 0; i < grids; i++)
	{
		write_pvd(*g[i], NULL, 0, (char *)"data/test", i, i);
	}

	return 0;
}
