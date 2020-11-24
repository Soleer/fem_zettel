/// @author Simon Gawlok

#ifndef _GRID_H_
#define _GRID_H_

#include <cmath>
#include <utility>
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>

#include "FE_VEC.h"

const int NDIM = 2;
const int NODES_PER_TRIANGLE = 3;

/*****************************************************************************/
/* Types                                                                     */
/*****************************************************************************/

/// @brief Structure for a node/point/vertex in a GRID
struct Coord
{

  /// Coordinates of the node/point/vertex
  double coords_[NDIM];
  Coord()
  {
  }

  Coord(double a, double b)
  {
    coords_[0] = a;
    coords_[1] = b;
  }
  /// Access the i-th coordinate of the node/point/vertex
  double &operator[](int i)
  {
    assert(i < NDIM);
    return coords_[i];
  }

  /// Access the i-th coordinate of the node/point/vertex
  double operator[](int i) const
  {
    assert(i < NDIM);
    return coords_[i];
  }

  Coord operator-(Coord minus)
  {
    Coord result(coords_[0] - minus[0], coords_[1] - minus[1]);
    return result;
  }

  Coord operator+(Coord plus)
  {
    Coord result(coords_[0] + plus[0], coords_[1] + plus[1]);
    return result;
  }

  Coord operator*(double d)
  {
    Coord result(coords_[0] * d, coords_[1] * d);
    return result;
  }
};

/// @brief Structure for a triangle in GRID
/// A triangle is defined by the numbers of the three nodes which are
/// the vertices of the triangle.
/// These vertices are numbered in counter-clockwise order if one looks
/// on the GRID from above
struct Triangle
{

  /// Array for the three vertices; entries have to be ordered in counter-
  /// clockwise order
  int vertices_[NODES_PER_TRIANGLE];
  Triangle()
  {
  }

  Triangle(int a, int b, int c)
  {
    vertices_[0] = a;
    vertices_[1] = b;
    vertices_[2] = c;
  }
  /// Access the number of the i-th vertex of the triangle
  /// Return value in the node number wrt. the GRID in which this triangle
  /// is contained
  int &operator[](int i)
  {
    assert(i < NODES_PER_TRIANGLE);
    return vertices_[i];
  }

  /// Access the number of the i-th vertex of the triangle
  /// Return value in the node number wrt. the GRID in which this triangle
  /// is contained
  int operator[](int i) const
  {
    assert(i < NODES_PER_TRIANGLE);
    return vertices_[i];
  }
};

/*****************************************************************************/
/* Structures                                                                */
/*****************************************************************************/

/// @brief Class for computational grid. Holds geometry information via
/// coordinates of points and information about the triangulation.
class GRID
{
private:
  /// Connectivity information, i.e. a vector of the triangles in
  /// the GRID
  std::vector<Triangle> conn_;

  /// Vector of nodes/points/vertices in the GRID.
  std::vector<Coord> coords_;

  /// Information about the refinement to the next finer level
  /// The first int (key) denotes a unique(!) ID for each edge in the GRID
  /// The second int (value) denotes the number of the newly created
  /// node in the specific edge wrt. to the node numbering in the finer GRID
  std::unordered_map<int, int> refinement_info_;

public:
  /// Default constructor
  GRID()
  {
  }

  /// Call several routines to intialize further data in GRID
  void init()
  {
  }

  /// Reserve memory for connectivity and coordinate vectors
  /// Useful, if there is a rough estimate for the final number of
  /// of entries in the respective vectors because it helps to prevent
  /// unnecessary reallocation of the content of the vectors during the
  /// addition of further entries
  void reserve(int num_tri, int num_vertex)
  {
    coords_.reserve(num_vertex);
    conn_.reserve(num_tri);
  }

  /// Get total number of nodes in Grid
  int num_nodes() const
  {
    return coords_.size();
  }

  /// Get total number of triangles in Grid
  int num_triangles() const
  {
    return conn_.size();
  }

  /// Get Triangle number tri_index
  Triangle &get_triangle(int tri_index)
  {
    assert(tri_index >= 0);
    assert(tri_index < static_cast<int>(conn_.size()));
    return conn_[tri_index];
  }

  /// Get Coordinates of vertex point_index
  Coord &get_coordinates(int point_index)
  {
    assert(point_index >= 0);
    assert(point_index < static_cast<int>(coords_.size()));
    return coords_[point_index];
  }

  /// Add Triangle
  /// New triangle will be added as "last" triangle in the GRID,
  /// i.e. its number is num_triangles() (before insertion) + 1
  void add_triangle(Triangle &new_tri)
  {
    for (int v = 0; v < NODES_PER_TRIANGLE; v++)
    {
      assert(new_tri[v] < static_cast<int>(coords_.size()));
    }
    conn_.push_back(new_tri);
  }

  /// Add Vertex
  /// New vertex will be added as "last" vertex in the GRID,
  /// i.e. its number is num_nodes() (before insertion) + 1
  void add_vertex(Coord &new_vertex)
  {
    coords_.push_back(new_vertex);
  }

  int edge_id(int v_start_id, int v_end_id)
  {
    //x * y + trunc((|x - y| - 1)^2 / 4)
    // https://gist.github.com/ma11hew28/9457506
    v_start_id++;
    v_end_id++;
    if (v_start_id < v_end_id)
    {
      return v_start_id * v_end_id + pow(v_end_id - v_start_id - 1, 2) / 4;
    }
    else
    {
      return v_start_id * v_end_id + pow(v_start_id - v_end_id - 1, 2) / 4;
    }
  }

  /// Read grid from given files
  /// Coordinates of vertices and connectivity information for the
  /// triangles are loaded from seperate files
  void read_from_file(
      const char *coords_filename,
      const char *conn_filename);

  /// Routine to uniformly refine the GRID and interpolate given FE_VECs on this GRID to new grid
  /// \param[in] in FE_VECs to be interpolated
  /// \param[in] num_vec number of inpute FE_VECs
  /// \param[out] newgrid refined GRID
  /// \param[out] out interpolated FE_VECs on newgrid
  void refine_ip(
      const FE_VEC in[],
      int num_vec,
      GRID &newgrid,
      FE_VEC out[]);

  /// Print out GRID
  inline void print(void) const
  {
    std::cout << "Points:" << std::endl;
    for (int i = 0; i < static_cast<int>(coords_.size()); ++i)
    {
      std::cout << "\t" << i << ": (" << coords_[i][0] << ", " << coords_[i][1] << ")" << std::endl;
    }

    std::cout << "Connectivity:" << std::endl;
    for (int i = 0; i < static_cast<int>(conn_.size()); ++i)
    {
      std::cout << "\t" << i << ": (" << conn_[i][0] << ", " << conn_[i][1] << ", " << conn_[i][2] << ")" << std::endl;
    }
  }
};

/*****************************************************************************/
/* Functions                                                                 */
/*****************************************************************************/

/// Write out FE_VECs on GRID to vtu file for visualization (use e.g. ParaView to view these files)
/// @param g GRID on which the FE_VECs are defined
/// @param vectors FE_VECs which data shall be visualized
/// @param n_vectors length of vectors
/// @param name filename for the output
int write_vtu(        // Function to write a grid to a .vtu-file for Paraview
    GRID &g,          // a grid
    FE_VEC vectors[], // a list of fe vectors
    int n_vectors,    // number of fe vectors to write
    char *name        // filename to write to, will overwrite w/o warning
);

/// Write out FE_VECs on GRID as one time-step of a time series via a pvd file (use e.g. ParaView to view this file)
/// Creates a vtu file for this time-step (use e.g. ParaView to view these files)
/// @param g GRID on which the FE_VECs are defined
/// @param vectors FE_VECs which data shall be visualized
/// @param n_vectors length of vectors
/// @param prefix prefix for filenames of pvd and vtu files
/// @param timestep number of the time-step to be visualized
/// @param time (physical/simulated) time of the time-step
int write_pvd(     // for problems that change over time this is useful:
    GRID &g,       // grid
    FE_VEC u[],    // fe vectors
    int n_vectors, // no of fe vectors
    char *prefix,  // prefix for filenames
    int timestep,  // number of timestep
    double time    // current time
);

#endif
