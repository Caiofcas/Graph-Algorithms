/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <cstdlib> // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <exception>
#include <iostream>
#include <tuple>   // for std::tie and std::ignore
#include <utility> // for pairs
#include <vector>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

/*========================================================
  ================ DIGRAPH DEFINITIONS ===================
  ========================================================*/

#define FORWARD 1
#define BACKWARD -1

struct BundledVertex
{
};

struct BundledArc
{
  short direction = FORWARD;
  unsigned short capacity, res_capacity, flow = 0;
  BundledArc *paired_arc = nullptr;
};

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    BundledVertex,
    BundledArc>
    Digraph;

typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;

struct FlowProblem
{
  Digraph d;
  Vertex source;
  Vertex sink;
  std::vector<Arc> original_order;
};

/*========================================================
  ============== END DIGRAPH DEFINITIONS =================
  ========================================================*/

/*========================================================
  ================ DIGRAPH UTILS =========================
  ========================================================*/

FlowProblem read_flow(std::istream &is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n;
  is >> n;

  FlowProblem fd;
  Digraph dig(n);
  Vertex source, sink;

  size_t m, i;

  is >> m;
  is >> source >> sink;

  i = 0;
  while (i < m)
  {
    int u, v;
    is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, dig);
    is >> dig[a].capacity;
    fd.original_order.push_back(a);
  }

  fd.d = dig;
  fd.source = source;
  fd.sink = sink;

  return fd;
}

void print_res_capacity(Digraph &d_hat, std::vector<Arc>& ordering)
{
  int forward_cap, backward_cap;
  for (Arc a : ordering){
    if (d_hat[a].ordering == FORWARD){
      forward_cap = d_hat[a].res_capacity;
      backward_cap = (*d_hat[a].paired_arc).res_capacity;
    } else {
      forward_cap = (*d_hat[a].paired_arc).res_capacity;
      backward_cap = d_hat[a].res_capacity;
    }
    std::cout << forward_cap << " " << backward_cap << std::endl;
  }

};

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/

int main(int argc, char **argv)
{
  FlowProblem flow_problem{read_flow(std::cin)};

  return EXIT_SUCCESS;
}