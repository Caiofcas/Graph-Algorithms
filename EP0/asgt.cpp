/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"
#include "arb.h"
#include <boost/graph/detail/adjacency_list.hpp>
#include <queue>
#include <vector>


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

HeadStart preprocess(Arb &arb, const Vertex& root)
{
  // HeadStart object will store two numbers per vertex:
	//   - depth (distance from the root)
	//   - closest_branching, last vertex of arb that has >1 out edge

  typedef boost::graph_traits<Arb>::vertex_iterator Vertex_it_type;

  HeadStart head = HeadStart(arb.vertex_set().size());
  Vertex v_it;
  std::vector<bool> visited(arb.vertex_set().size(), false);
  std::queue<Vertex> q;
  int depth = 0;

  //Start by processing root
  head.depth[root] = depth++;
  head.closest_branching[root] = 0;
  q.push(root);

  //Perform a DFS through the vertices
  do{
    v_it = q.front();
    q.pop();

    //
    if (!visited[v_it]){
      visited[v_it] = true;
      head.depth[v_it] = depth;
      
    }

    ++depth;
  } while(!q.empty());

  return head;
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
