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
#include <boost/optional.hpp>

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
  Arc paired_arc;
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
  ================= PATH DEFINITIONS =====================
  ========================================================*/

/* Path through Residual Digraph */
class Path
{
public:
  /* start building a walk in digraph from vertex start */
  Path(const ResDigraph &digraph, const Vertex &start);

  /* extend the walk by adding arc to its end */
  bool extend(const Arc &arc);

  /* Check before adding vertex */
  bool already_seen(const Vertex v);

  /* accessors */
  const ResDigraph &digraph() const { return _digraph.get(); }
  const std::vector<Arc> &arcs() const { return _arcs; }

  friend std::ostream &operator<<(std::ostream &os, const Path &path);

private:
  std::reference_wrapper<const ResDigraph> _digraph;
  const Vertex _start;
  Vertex _last;
  std::vector<Arc> _arcs;
  std::vector<bool> _visited;
};

/* Walk constructor */
Path::Path(const ResDigraph &digraph, const Vertex &start)
    : _digraph(std::cref(digraph)),
      _start(start),
      _last(start),
      _visited(std::vector<bool>(boost::num_vertices(digraph), false))
{
  _visited[start] = true;
}

bool Path::extend(const Arc &arc)
{
  Vertex u = boost::source(arc, digraph());
  if (u != _last)
  {
    return false;
  }
  Vertex v = boost::target(arc, digraph());
  if (_visited[v])
    return false;

  _arcs.push_back(arc);
  _visited[v] = true;
  _last = v;
  return true;
}

bool Path::already_seen(const Vertex v)
{
  return _visited[v];
}

/*========================================================
  ================ END PATH DEFINITIONS ==================
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
    //TODO: check if paired arc exists
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

std::tuple<
    bool,
    boost::optional<Path>>
    //boost::optional<ArcSet> : reachable arcs
find_min_path(ResDigraph &d_hat, Vertex start){

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
      rd[a_f].paired_arc = a_f;
      i++;
    }
    if (d[*a_it].flow > 0)
    {
      tie(a_b, std::ignore) = boost::add_edge(v, u, rd);
      rd[a_b].direction = BACKWARD;
      rd[a_b].res_capacity = d[*a_it].flow;
      rd[a_b].paired_arc = a_b;
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

void edmonds_karp(FlowProblem &fp)
{

  int t = 0;
  while (true)
  {
    // 1. Compute residual digraph of D, d_hat
    ResDigraph d_hat = build_res_digraph(fp.d);

    bool reaches_sink;
    auto ret = find_min_path(d_hat, fp.source);
    if (std::get<bool>(ret)){
      Path p = std::get<Path>(ret); 
      // source-sink path exists
      //  3.2 Get eps = min(res_c(arc) for arc in P)
      //  3.3 f_base += eps * path_flow
    } else {
      // 2. Build Set S of vertices reached by source
      //  3.1 Return (S, f_base)
    }
    t++;
  }
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