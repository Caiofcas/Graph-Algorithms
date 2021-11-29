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
  unsigned long capacity;
  BundledArc() : capacity(0) {}
};

/* the remainder of the file must not be changed */
typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS,
                              BundledVertex,
                              BundledArc> Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;

/*========================================================
  ============== END DIGRAPH DEFINITIONS =================
  ========================================================*/

/*========================================================
  ================ DIGRAPH UTILS =========================
  ========================================================*/

Digraph read_digraph(std::istream& is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n; is >> n;

  Digraph dig(n);

  size_t m; is >> m;

  Vertex source, sink; is >> source >> sink;

  while (m--) {
    int u, v; is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, dig);
    is >> dig[a].capacity;
  }

  return dig;
}

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/



int main(int argc, char** argv)
{
  Digraph digraph{read_digraph(std::cin)};

  return EXIT_SUCCESS;
}