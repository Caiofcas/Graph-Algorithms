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
  Vertex u;

  hst->d[*v] = ++(*time);
  visited[*v] = GRAY;
  printf("Visiting %ld [%d]\n",*v, *time);

  for(std::tie(out_edge_it, out_edge_end) = boost::out_edges(*v, arb);
      out_edge_it != out_edge_end; ++out_edge_it){
        u = boost::target(*out_edge_it, arb);
        if (visited[u] == WHITE) dfs_visit(&u, visited, time, hst, arb);
      };

  hst->f[*v] = ++(*time);
  visited[*v] = BLACK;
  printf("Exiting %ld [%d]\n",*v, *time);
}

HeadStart preprocess(Arb &arb, const Vertex& root)
{
  // Performs DF-Search and stores two vectors:
  // hst.d[i] = moment vertex i starts being processed
  // hst.f[i] = moment vertex i stops being processed

  HeadStart hst = HeadStart(arb.m_vertices.size());
  std::vector<int> visited(arb.m_vertices.size(), WHITE);
  int time = 0;

  //Start by processing root
  dfs_visit(&root, visited, &time, &hst, arb);

  printf("Exiting preprocess.\n");
  return hst;
}

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
{
  // is u an ancestor of v
  printf("Is %ld an ancestor of %ld?\n", u, v);
  if (data.d[u] <= data.d[v] && data.f[v] <= data.f[u]) return true;
  return false;
}
