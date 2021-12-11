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

using std::tie;

/*========================================================
  ================ DIGRAPH DEFINITIONS ===================
  ========================================================*/

#define FORWARD 1
#define BACKWARD -1

// Regular Digraph

struct DigraphBundledVertex
{
};

struct DigraphBundledArc
{
  unsigned short capacity, flow = 0;
};

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    DigraphBundledVertex,
    DigraphBundledArc>
    Digraph;

typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;

// Residual Digraph

struct ResDigraphBundledVertex
{
};

struct ResDigraphBundledArc
{
  short direction = FORWARD;
  unsigned short res_capacity = 0;
  Arc paired_arc = nullptr;
};

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    ResDigraphBundledVertex,
    ResDigraphBundledArc>
    ResDigraph;

// Problem

struct FlowProblem
{
  Digraph d;
  Vertex source;
  Vertex sink;
  std::vector<Arc> arc_order;
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
    fd.arc_order.push_back(a);
  }

  fd.d = dig;
  fd.source = source;
  fd.sink = sink;

  return fd;
}

void print_res_capacity(ResDigraph &d_hat, std::vector<Arc> &ordering)
{
  int forward_cap, backward_cap;
  for (Arc a : ordering)
  {
    if (d_hat[a].direction == FORWARD)
    {
      forward_cap = d_hat[a].res_capacity;
      backward_cap = d_hat[d_hat[a].paired_arc].res_capacity;
    }
    else
    {
      forward_cap = d_hat[d_hat[a].paired_arc].res_capacity;
      backward_cap = d_hat[a].res_capacity;
    }
    std::cout << forward_cap << " " << backward_cap << std::endl;
  }
};

// TODO: preserve ordering
ResDigraph build_res_digraph(Digraph &d)
{
  ResDigraph rd;
  Digraph::edge_iterator a_it, a_end;

  for (tie(a_it, a_end) = boost::edges(d); a_it != a_end; a_it++)
  {
    short i = 0; // keep track of edges added
    int res_capacity = d[*a_it].capacity - d[*a_it].flow;
    Arc a_f, a_b;
    Vertex u = (*a_it).m_source, v = (*a_it).m_target;

    if (res_capacity > 0)
    {
      tie(a_f, std::ignore) = boost::add_edge(u, v, rd);
      rd[a_f].direction = FORWARD;
      rd[a_f].res_capacity = res_capacity;
      i++;
    }
    if (d[*a_it].flow > 0)
    {
      tie(a_b, std::ignore) = boost::add_edge(v, u, rd);
      rd[a_b].direction = BACKWARD;
      rd[a_b].res_capacity = d[*a_it].flow;
      i++;
    }
    if (i == 2)
    {
      rd[a_b].paired_arc = a_f;
      rd[a_f].paired_arc = a_b;
    }
  }

  return rd;
}

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/

/*========================================================
  =================== Edmonds-Karp =======================
  ========================================================*/

void edmonds_karp(FlowProblem &fp){

};

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/

int main(int argc, char **argv)
{
  FlowProblem flow_problem{read_flow(std::cin)};

  // is this by copy?
  edmonds_karp(flow_problem);

  return EXIT_SUCCESS;
}