#include <stdlib.h>
#include <stdio.h>
#include "sys/time.h"
#include <iostream>
#include <assert.h>

#include "grid.h"
#include "FE_VEC.h"

#include "exercise_sheet_2.h"
#include "exercise_sheet_3.h"

#include "config.h"

int main(int argc, char *argv[]) {

	timeval solstart, solende;
	double start_s, end_s;
	GRID *g[grids];

	// On each grid level: 0 boundary_flag, 1 set Dirichlet boundary conditon
	std::vector<FE_VEC*> values(grids);

	std::vector<int> dirichlet_nodes;
	std::vector<double> dirichlet_val;

	for(int i = 0; i < grids; ++i) {
		g[i] = new GRID;
		assert(g[i] != NULL);

		values[i] = new FE_VEC[2];

		values[i][0].setName((char*) "Boundary flag");
		values[i][1].setName((char*) "Dirichlet BC");
	}

	// Initial GRID level
	std::cout << "====================================================" << std::endl;
	std::cout << "Level 0" << std::endl;
	std::cout << "====================================================" << std::endl;

	// read initial grid from file
	g[0]->read_from_file(coord_filename, conn_filename);

	// prepare function values
	values[0][1].resize(g[0]->num_nodes());

	// get boundary flag
	g[0]->boundary_flag_to_FE_VEC(values[0][0]);

	// compute Dirichlet BC
	gettimeofday(&solstart, NULL);
	g[0]->compute_dirichlet_nodes_and_values(Dirichlet_BC_exercise_3, dirichlet_nodes, dirichlet_val);
	gettimeofday(&solende, NULL);

	start_s = solstart.tv_sec + solstart.tv_usec * 1.0e-6;
	end_s = solende.tv_sec + solende.tv_usec * 1.0e-6;
	std::cout << std::endl << "Computation of Dirichlet boundary conditions took " << end_s - start_s << " seconds." << std::endl;

	values[0][1].setValues(dirichlet_val, dirichlet_nodes);

	// refine grid grid-1 times and interpolate values to next level
	for (int i = 1; i < grids; ++i) {
		std::cout << "====================================================" << std::endl;
		std::cout << "Level " << i << std::endl;
		std::cout << "====================================================" << std::endl;
		// refine grid and interpolate values
		gettimeofday(&solstart, NULL);
		g[i-1]->refine_ip(NULL, 0, *g[i], NULL);
		gettimeofday(&solende, NULL);

		start_s = solstart.tv_sec + solstart.tv_usec * 1.0e-6;
		end_s = solende.tv_sec + solende.tv_usec * 1.0e-6;
		std::cout << std::endl << "The refinement from level " << i-1 << " to level " << i << " (including calculation of boundary_flag_) took " << end_s - start_s << " seconds." << std::endl;

		std::cout << "Number of nodes on level " << i <<": " << g[i]->num_nodes() << std::endl;
		std::cout << "Number of triangles on level " << i << ": " << g[i]->num_triangles() << std::endl;

		// prepare function values on new grid level
		values[i][1].resize(g[i]->num_nodes());

		// get boundary flag
		g[i]->boundary_flag_to_FE_VEC(values[i][0]);

		// compute Dirichlet BC
		gettimeofday(&solstart, NULL);
		g[i]->compute_dirichlet_nodes_and_values(Dirichlet_BC_exercise_3, dirichlet_nodes, dirichlet_val);
		gettimeofday(&solende, NULL);

		start_s = solstart.tv_sec + solstart.tv_usec * 1.0e-6;
		end_s = solende.tv_sec + solende.tv_usec * 1.0e-6;
		std::cout << std::endl << "Computation of Dirichlet boundary conditions took " << end_s - start_s << " seconds." << std::endl;
		
		values[i][1].setValues(dirichlet_val, dirichlet_nodes);
	}

	// Visualize the results
	for (int i = 0; i < grids; ++i) {
		write_pvd(*g[i], values[i], 2, (char*) "data/test", i, i);
	}

 	return 0;
}
