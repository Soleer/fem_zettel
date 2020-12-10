/// @author Simon Gawlok

#ifndef _EXERCISE_SHEET_2_H_
#define _EXERCISE_SHEET_2_H_

#include <cassert>
#include <cmath>

/// Function to compute exact function values for exercise sheet 2
void compute_function_sheet_2(GRID &g, FE_VEC &vec) {
    assert(g.num_nodes() == vec.length());
    
    for(int i = 0; i < g.num_nodes(); ++i) {
        Coord pt_i = g.get_coordinates(i);
        vec[i] = std::sin(M_PI * pt_i[0]) * std::exp(pt_i[1]);
    }
}

#endif
