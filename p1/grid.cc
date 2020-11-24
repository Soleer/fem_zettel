/// @author Simon Gawlok

#include <fstream>
#include <sstream>
#include "grid.h"

void GRID::read_from_file(
  const char* coords_filename,
  const char* conn_filename
) {
  
  // ifstream to read coordinate data, automatically opens coords_filename
  std::ifstream coords(coords_filename);
  
  // ifstream to read coordinate data, automatically opens coords_filename
  std::ifstream conns(conn_filename);
  
  // check if coords_filename exists and read in data
  if (!coords.is_open()) {
    std::cout << "Could not open " << coords_filename << " for reading coordinates." << std::endl;
    exit(-1);
  } else {
    // read point coordinates
    while (!coords.eof()) {
      Coord temp;
      std::string str;
      std::getline(coords, str);
      std::stringstream ss(str);
      int component = 0;
      while(component < NDIM && ss >> temp[component]) {
        ++component;
      }
      this->add_vertex(temp);
    }
    coords_.pop_back();
    coords.close();
  }
  
  // check if conn_filename exists and read in data
  if (!conns.is_open()) {
    std::cout << "Could not open " << conn_filename << " for reading connectivity." << std::endl;
    exit(-1);
  } else {
    // read point coordinates
    while (!conns.eof()) {
      Triangle temp;
      std::string str;
      std::getline(conns, str);
      std::stringstream ss(str);
      int component = 0;
      while(component < NODES_PER_TRIANGLE && ss >> temp[component]) {
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
  FE_VEC out[]
){
  
  // HOMEWORK: implement your refinement algorithm here
  
  
  // Initialize further data on newgrid
  newgrid.init();
}
