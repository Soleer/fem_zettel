/// @author Simon Gawlok

#ifndef _EXERCISE_SHEET_3_H_
#define _EXERCISE_SHEET_3_H_

#include <cassert>
#include <cmath>

/// Function to compute Dirichlet boundary condition for practical exercise 3
/// @param[in] g GRID on which Dirichlet BC is computed
/// @param[in] pts indices of points on current boundary edge
/// @param[out] dirichlet_val if empty, no Dirichlet BC at given points; otherwise, contains Dirichlet BC at given points (both!!)
void Dirichlet_BC_exercise_3(GRID &g, const std::vector<int> &pts, std::vector<double> &dirichlet_val) {
    dirichlet_val.clear();
    if((g.get_coordinates(pts[0])[1] == 1.0 && g.get_coordinates(pts[1])[1] == 1.0) || (g.get_coordinates(pts[0])[0] == 1.0 && g.get_coordinates(pts[1])[0] == 1.0)) {
        dirichlet_val.resize(2);
        dirichlet_val[0] = std::cos(g.get_coordinates(pts[0])[0]) * std::cos(g.get_coordinates(pts[0])[1]);
        dirichlet_val[1] = std::cos(g.get_coordinates(pts[1])[0]) * std::cos(g.get_coordinates(pts[1])[1]);
    }
}

#endif
