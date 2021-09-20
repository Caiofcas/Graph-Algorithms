/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"
#include "arb.h"
#include <boost/graph/detail/adjacency_list.hpp>
#include <queue>
#include <vector>

#define WHITE 0
#define GRAY 1
#define BLACK 2

Arb read_arb(std::istream& in)
{
  size_t n_vertex; 
  std::cin >> n_vertex;
  Arb arb(n_vertex);

  // Edges are read to the n-th line, but one line has already been read
  n_vertex--;

  while(n_vertex--){
    Vertex x, y; std::cin >> x >> y;
    boost::add_edge(--x, --y, arb);
    //std::cout << ++x << "  " << ++y << std::endl;
  }

  /*print read edges
  Arb::edge_iterator edge;
  std::pair<Arb::edge_iterator,Arb::edge_iterator> edges = boost::edges(arb);
  for (edge = edges.first; edge != edges.second; ++edge){
    std::cout << *edge << std::endl;
  }
  */
  return arb;
}


void dfs_visit(const Vertex* v, std::vector<int> visited, int* time, HeadStart* hst, Arb &arb){
  
  typename boost::graph_traits<Arb>::out_edge_iterator out_edge_it, out_edge_end;

  hst->d[*v] = (*time)++;
  visited[*v] = GRAY;

  for(std::tie(out_edge_it, out_edge_end) = boost::out_edges(*v, arb);
      out_edge_it != out_edge_end; ++out_edge_it){
        
      }
}

HeadStart preprocess(Arb &arb, const Vertex& root)
{
  // Performs DF-Search and stores two vectors:
  // hst.d[i] = moment vertex i starts being processed
  // hst.f[i] = moment vertex i stops being processed

  HeadStart hst = HeadStart(arb.vertex_set().size());
  std::vector<int> visited(arb.vertex_set().size(), WHITE);
  int time = 0;

  //Start by processing root
  dfs_visit(&root, visited, &time, &hst, arb);

  return hst;
}

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
{ 

  // v must be shallower than u
  if (data.depth[u] <= data.depth[v])
    return false;

	if (data.closest_branching[u] == data.closest_branching[v])
    return true;

  Vertex branching = data.closest_branching[u];

	//if closest_branching is not the same, need to check their depth

  while (data.depth[branching] < data.depth[v]){
    if (branching == v) return true;
    branching = data.closest_branching[branching];
  };

  if (data.depth[branching] == data.depth[v]) return false;

  return false;
}
