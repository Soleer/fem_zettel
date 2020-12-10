/// @author Simon Gawlok

#include <fstream>
#include <sstream>
#include <map>
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
) {

  // node numbers in old grid on each triangle
  int  o1, o2, o3;

  // IDs of edges in old grid on each triangle
  int entry1, entry2, entry3;

  // Iterators in refinement_info_ for each edge.
  // Points to the corresponding entry for edge{1, 2, 3} in refinement info
  // if edge has already been refined; otherwise, it points to the end of
  // refinement_info_
  std::unordered_map<int, int>::iterator k1, k2, k3;

  // node numbers of newly added nodes on each triangle (wrt. old grid)
  int node1, node2, node3;


  // reserve memory in newgrid
  // newgrid contains 4 times the number of triangles of old grid
  // in worst case, on each triangle three new nodes are created
  newgrid.reserve(4 * this->num_triangles(), this->num_nodes() + 3 * this->num_triangles());

  // check for compatibility of input FE_VECs and old grid
  for(int k = 0; k < num_vec; ++k) {
    if(in[k].length() != this->num_nodes()){
      std::cout << "Number of nodes in input grid (" << this->num_nodes() << ") and number of elements in input FE_VEC " << k << " ( " << in[k].length() << ") must match." << std::endl;
      return;
    }
  }

  // copy already existing nodes to newgrid
  for(int j = 0; j < this->num_nodes(); ++j) {
    newgrid.add_vertex(this->get_coordinates(j));
  }

  // allocate enough memory for output FE_VECs
  // -> in worst case, on each triangle three new nodes are created
  // copy values of input FE_VECs to output FE_VECs
  for(int k = 0; k < num_vec; ++k){
    out[k].resize(this->num_nodes() + 3 * this->num_triangles());
    for(int j = 0; j < in[k].length(); ++j) {
      out[k][j] = in[k][j];
    }
  }


  // loop over all triangles
  for(int i = 0; i < this->num_triangles(); i++){

    //get nodes in triangle i of old grid
    o1 = this->get_triangle(i)[0];
    o2 = this->get_triangle(i)[1];
    o3 = this->get_triangle(i)[2];

    // sort the starting and ending nodes and compute ID unique to each edge
    // Each edge can uniquely be enumerated by
    // ID = min(node_number_1, node_number_2) * num_nodes_old + max(node_number_1, node_number_2)
    // where node_number_{1, 2} denote the numbers of the nodes defining
    // the edge
    if(o1 > o2) {
      entry1 = o2 * this->num_nodes() + o1;
    } else {
      entry1 = o1 * this->num_nodes() + o2;
    }
    if(o2 > o3) {
      entry2 = o3 * this->num_nodes() + o2;
    } else {
      entry2 = o2 * this->num_nodes() + o3;
    }
    if(o3 > o1) {
      entry3 = o1 * this->num_nodes() + o3;
    } else {
      entry3 = o3 * this->num_nodes() + o1;
    }

    // check if needed nodes already exist
    k1 = this->refinement_info_.find(entry1);
    k2 = this->refinement_info_.find(entry2);
    k3 = this->refinement_info_.find(entry3);


    // if new node does not already exist it is created and added to newgrid
    // don't forget to remember number of new node

    // first edge
    if(k1 == this->refinement_info_.end()) { // node in the edge does NOT already exist

      // insert edge ID and number of newly created node
      this->refinement_info_.insert(std::pair<int, int>(entry1, newgrid.num_nodes()));

      // create new node and add it to newgrid
      Coord new_vertex;
      new_vertex[0] = (this->get_coordinates(o1)[0] + this->get_coordinates(o2)[0]) * 0.5;
      new_vertex[1] = (this->get_coordinates(o1)[1] + this->get_coordinates(o2)[1]) * 0.5;
      newgrid.add_vertex(new_vertex);

      // interpolate values to new node via linear interpolation between
      // the values of the nodes defining the edge in the old grid
      for(int j = 0; j < num_vec; ++j) {
        out[j][newgrid.num_nodes()-1] = (out[j][o1] + out[j][o2]) * 0.5;
      }

      // remember number of new node
      node1 = newgrid.num_nodes()-1;
    } else { // node DOES already exist

      // get number of already existing new node
      node1 =k1->second;
    }

    // second edge
    if(k2 == this->refinement_info_.end()) { // node in the edge does NOT already exist

      // insert edge ID and number of newly created node
      this->refinement_info_.insert(std::pair<int, int>(entry2, newgrid.num_nodes()));

      // create new node and add it to newgrid
      Coord new_vertex;
      new_vertex[0] = (this->get_coordinates(o2)[0] + this->get_coordinates(o3)[0]) * 0.5;
      new_vertex[1] = (this->get_coordinates(o2)[1] + this->get_coordinates(o3)[1]) * 0.5;
      newgrid.add_vertex(new_vertex);

      // interpolate values to new node via linear interpolation between
      // the values of the nodes defining the edge in the old grid
      for(int j = 0; j < num_vec; ++j) {
        out[j][newgrid.num_nodes()-1] = (out[j][o2] + out[j][o3]) * 0.5;
      }

      // remember number of new node
      node2 = newgrid.num_nodes()-1;
    } else { // node DOES already exist

      // get number of already existing new node
      node2 = k2->second;
    }

    // third edge
    if(k3 == this->refinement_info_.end()) { // node in the edge does NOT already exist

      // insert edge ID and number of newly created node
      this->refinement_info_.insert(std::pair<int, int>(entry3, newgrid.num_nodes()));

      // create new node and add it to newgrid
      Coord new_vertex;
      new_vertex[0] = (this->get_coordinates(o3)[0] + this->get_coordinates(o1)[0]) * 0.5;
      new_vertex[1] = (this->get_coordinates(o3)[1] + this->get_coordinates(o1)[1]) * 0.5;
      newgrid.add_vertex(new_vertex);

      // interpolate values to new node via linear interpolation between
      // the values of the nodes defining the edge in the old grid
      for(int j = 0; j < num_vec; ++j) {
        out[j][newgrid.num_nodes()-1] = (out[j][o3] + out[j][o1]) * 0.5;
      }

      // remember number of new node
      node3 = newgrid.num_nodes()-1;
    } else { // node DOES already exist

      // get number of already existing new node
      node3 = k3->second;
    }


    //create the new triangles and them to newgrid

    // first triangle
    Triangle new_tri_1;
    new_tri_1[0] = o1;
    new_tri_1[1] = node1;
    new_tri_1[2] = node3;
    newgrid.add_triangle(new_tri_1);

    // second triangle
    Triangle new_tri_2;
    new_tri_2[0] = node1;
    new_tri_2[1] = node2;
    new_tri_2[2] = node3;
    newgrid.add_triangle(new_tri_2);

    // third triangle
    Triangle new_tri_3;
    new_tri_3[0] = node1;
    new_tri_3[1] = o2;
    new_tri_3[2] = node2;
    newgrid.add_triangle(new_tri_3);

    // fourth triangle
    Triangle new_tri_4;
    new_tri_4[0] = node3;
    new_tri_4[1] = node2;
    new_tri_4[2] = o3;
    newgrid.add_triangle(new_tri_4);
  }

  // resive ouput FE_VECs to final (and correct) size
  // i.e. number of nodes in newgrid
  for(int j = 0; j < num_vec; j++){
    out[j].resize(newgrid.num_nodes());
  }

  // Initialize further data on newgrid
  newgrid.init();
}


void GRID::compute_boundary_flag() {
  
  // clear possible old values in boundary_flag_
  boundary_flag_.clear();
  // initialize boundary_flag_ with false
  boundary_flag_.resize(num_nodes(), false);
  Triangle t;
  std::vector<int> t_count(num_nodes(), 0);
  // count number of triangles every node is contained in
  for(int i= 0; i< num_triangles(); i++){
    t = get_triangle(i);
    for(int i=0; i< NODES_PER_TRIANGLE; i++){
      t_count[t[i]]++; 
    }
  }
  // if a node is in one or three triangles, it must be on the border
  for(int i=0; i<num_nodes(); i++){
    boundary_flag_[i] = t_count[i] <= 3;
  }

}

void GRID::compute_dirichlet_nodes_and_values(void (*function_to_call)(GRID&, const std::vector<int>&, std::vector<double>&),
                                        std::vector<int> &dirichlet_nodes,
                                        std::vector<double> &dirichlet_val)
{
  Triangle t;
  int a, b;
  std::vector<int> bd_pts(2);
  std::vector<bool> is_already(num_nodes());
  //iter ofer triangles
  for(int i = 0; i<num_triangles(); i++){
    t = get_triangle(i);
    // iter over edges
    for(int e = 0; e < NODES_PER_TRIANGLE; e++ ){
      a = t[e];
      b = t[(e+1) % NODES_PER_TRIANGLE];
      //check if edge is on boundary
      if(boundary_flag_[a] && boundary_flag_[b]){
        std::vector<double> bd_vals;
        bd_pts[0] = a;
        bd_pts[1] = b;
        (*function_to_call)(*this, bd_pts, bd_vals);
        if(bd_vals.size()==2){
          if(!is_already[a]){
            dirichlet_nodes.push_back(a);
            dirichlet_val.push_back(bd_vals[0]);
            is_already[a] = true;
          }
          if(!is_already[b]){
              dirichlet_nodes.push_back(b);
              dirichlet_val.push_back(bd_vals[1]);
              is_already[b] = true;
          } 
        }
      }
    }
  }
}
