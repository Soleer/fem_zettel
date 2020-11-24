/// @author Simon Gawlok

#include <fstream>
#include <sstream>
#include "grid.h"

void GRID::read_from_file(
    const char *coords_filename,
    const char *conn_filename)
{

  // ifstream to read coordinate data, automatically opens coords_filename
  std::ifstream coords(coords_filename);

  // ifstream to read coordinate data, automatically opens coords_filename
  std::ifstream conns(conn_filename);

  // check if coords_filename exists and read in data
  if (!coords.is_open())
  {
    std::cout << "Could not open " << coords_filename << " for reading coordinates." << std::endl;
    exit(-1);
  }
  else
  {
    // read point coordinates
    while (!coords.eof())
    {
      Coord temp;
      std::string str;
      std::getline(coords, str);
      std::stringstream ss(str);
      int component = 0;
      while (component < NDIM && ss >> temp[component])
      {
        ++component;
      }
      this->add_vertex(temp);
    }
    coords_.pop_back();
    coords.close();
  }

  // check if conn_filename exists and read in data
  if (!conns.is_open())
  {
    std::cout << "Could not open " << conn_filename << " for reading connectivity." << std::endl;
    exit(-1);
  }
  else
  {
    // read point coordinates
    while (!conns.eof())
    {
      Triangle temp;
      std::string str;
      std::getline(conns, str);
      std::stringstream ss(str);
      int component = 0;
      while (component < NODES_PER_TRIANGLE && ss >> temp[component])
      {
        temp[component] -= 1;
        ++component;
      }
      this->add_triangle(temp);
    }
    conn_.pop_back();
    conns.close();
  }

  init();
}

void GRID::refine_ip(
    const FE_VEC in[],
    int num_vec,
    GRID &newgrid,
    FE_VEC out[])
{
  //add all current vertices to new grid and therfore keep ids the same
  for(int v = 0; v < this->num_nodes(); v++)
  {
    newgrid.add_vertex(this->get_coordinates(v));
  }
  // iterate over all triangles and add 4 new
  for(int t_id = 0; t_id < this->num_triangles(); t_id++)
  {
    Triangle current_triangle = this->get_triangle(t_id);
    // first add the new vertices for every edge of triangle if not already
    for(int v = 0; v < NODES_PER_TRIANGLE; v++)
    {
      int v_start_id = current_triangle[v];
      int v_end_id = current_triangle[(v+1) % NODES_PER_TRIANGLE];
      int edge_index = this->edge_id(v_start_id, v_end_id);
      if(refinement_info_.find(edge_index) == refinement_info_.end())
      {
        // compute vertex in the middle
        Coord new_vertex = this->get_coordinates(v_start_id) + (this->get_coordinates(v_end_id) - this->get_coordinates(v_start_id))* (1./2.);
        newgrid.add_vertex(new_vertex);
        refinement_info_[edge_index] = newgrid.num_nodes() -1;
      }
    }
    // add four triangles:
    int a = refinement_info_[this->edge_id(current_triangle[0],current_triangle[1])];
    int b = refinement_info_[this->edge_id(current_triangle[2],current_triangle[0])];
    int c = refinement_info_[this->edge_id(current_triangle[1],current_triangle[2])];
    Triangle A(current_triangle[0], a, b);
    Triangle B( a, current_triangle[1], c);
    Triangle C( b, c, current_triangle[2]);
    Triangle D( a, c, b);
    newgrid.add_triangle(A);
    newgrid.add_triangle(B);
    newgrid.add_triangle(C);
    newgrid.add_triangle(D); 
  }

  // Initialize further data on newgrid
  newgrid.init();
}
