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
  }

  return arb;
}


void dfs_visit(const Vertex* v, std::vector<int>* visited, int* time, HeadStart* hst, Arb &arb){
  
  typename boost::graph_traits<Arb>::out_edge_iterator out_edge_it, out_edge_end;
  Vertex u;

  hst->d[(int)*v] = (*time)++;

  (*visited)[(int)*v] = GRAY;

  for(std::tie(out_edge_it, out_edge_end) = boost::out_edges(*v, arb);
      out_edge_it != out_edge_end; ++out_edge_it){
        u = boost::target(*out_edge_it, arb);
        if ((*visited)[u] == WHITE) dfs_visit(&u, visited, time, hst, arb);
      };

  hst->f[(int)*v] = (*time)++;
  (*visited)[(int)*v] = BLACK;
}

HeadStart preprocess(Arb &arb, const Vertex& root)
{
  // Performs DF-Search and stores two vectors:
  // hst.d[i] = moment vertex i starts being processed
  // hst.f[i] = moment vertex i stops being processed

  HeadStart hst;
  int n = arb.m_vertices.size(), time = 0;
  std::vector<int> visited(n, WHITE);

  hst.d =(int*) malloc(n*sizeof(int));
  hst.f =(int*) malloc(n*sizeof(int));

  //Start by processing root
  dfs_visit(&root, &visited, &time, &hst, arb);

  return hst;
}

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
{
  // is u an ancestor of v
  if (data.d[u] <= data.d[v] && data.f[v] <= data.f[u]) return true;
  return false;
}
