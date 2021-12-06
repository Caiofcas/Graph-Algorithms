#include <cstdlib>              // size_t, EXIT_SUCCESS, and EXIT_FAILURE
#include <vector>
#include <iostream>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>


/*========================================================
  ================ DIGRAPH DEFINITIONS ===================
  ========================================================*/

struct BundledVertex
{

};

struct BundledArc
{
  unsigned short capacity;
  unsigned short order;
  BundledArc() : capacity(0) {}
};

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS,
                              BundledVertex,
                              BundledArc> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;


struct FlowProblem {
  Digraph d;
  Vertex source;
  Vertex sink;
};

/*========================================================
  ============== END DIGRAPH DEFINITIONS =================
  ========================================================*/

/*========================================================
  ================ DIGRAPH UTILS =========================
  ========================================================*/

FlowProblem read_flow(std::istream& is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n; is >> n;

  FlowProblem fd;
  Digraph dig(n);
  Vertex source, sink;

  size_t m, i;

  is >> m;
  is >> source >> sink;

  i = 0;
  while (i < m) {
    int u, v; is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, dig);
    is >> dig[a].capacity;
    dig[a].order = i++;
  }

  fd.d = dig;
  fd.source = source;
  fd.sink = sink;

  return fd;
}

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/



int main(int argc, char** argv)
{
  FlowProblem flow_problem{read_flow(std::cin)};

  return EXIT_SUCCESS;
}