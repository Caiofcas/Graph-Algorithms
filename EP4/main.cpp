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
#include <queue>

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

// Augmented Digraph

struct AugDigraphBundledVertex
{
};

struct AugDigraphBundledArc
{
  short direction = FORWARD;
  unsigned short res_capacity = 0;
  int orig_arc_idx;
};

typedef boost::adjacency_list<
    boost::vecS,
    boost::vecS,
    boost::directedS,
    AugDigraphBundledVertex,
    AugDigraphBundledArc>
    AugDigraph;

typedef boost::graph_traits<AugDigraph>::edge_descriptor AugArc;

// Regular Digraph

struct DigraphBundledVertex
{
};

struct DigraphBundledArc
{
  unsigned short capacity, flow = 0;
  AugArc *f_arc = nullptr, *b_arc = nullptr;
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

// Problem

struct FlowProblem
{
  Digraph d;
  AugDigraph a_d;
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
  AugDigraph aug_dig(n);
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
    AugArc a_f, a_b;
    std::tie(a, std::ignore) = boost::add_edge(u-1, v-1, dig);
    std::tie(a_f, std::ignore) = boost::add_edge(u-1, v-1, aug_dig);
    std::tie(a_b, std::ignore) = boost::add_edge(v-1, u-1, aug_dig);

    is >> dig[a].capacity;

    aug_dig[a_f].direction = FORWARD;
    aug_dig[a_b].direction = BACKWARD;
    aug_dig[a_f].orig_arc_idx = aug_dig[a_b].orig_arc_idx = i;
    fd.arc_order.push_back(a);
    i++;
  }

  fd.a_d = aug_dig;
  fd.d = dig;
  fd.source = source;
  fd.sink = sink;

  return fd;
}

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/

/*========================================================
  =================== Edmonds-Karp =======================
  ========================================================*/

// Returns:
//  - (true, Arcs representing path from start to target, none)
//  - (false, none, Vertices reachable from start)
std::tuple<
    bool,
    boost::optional<std::vector<Arc>>,
    boost::optional<std::vector<Vertex>>>
find_min_path(AugDigraph &d_hat, Vertex start, Vertex target)
{

  // BFS

  std::vector<bool> reached(boost::num_vertices(d_hat), false);
  std::vector<Arc> reached_by(boost::num_vertices(d_hat));
  std::queue<Vertex> q;
  bool found = false;

  q.push(start);
  reached[start] = true;

  while (!q.empty() && !found)
  {
    // get next element
    Vertex v = q.front();
    q.pop();

    AugDigraph::edge_iterator a_it, a_end;

    // get adjacent vertices
    for (tie(a_it, a_end) = boost::out_edges(v, d_hat);
         a_it != a_end; a_it++)
    {
      Vertex u = boost::target(*a_it, d_hat);
      q.push(u);
      if (!reached[u])
      {
        reached[u] = true;
        reached_by[u] = *a_it;
        if (u == target)
        {
          found = true;
          break;
        }
      }
    }
  }

  if (found)
  {
    std::vector<Arc> path;
    Vertex u = target;
    while (u != start)
    {
      path.push_back(reached_by[u]);
      u = boost::source(reached_by[u], d_hat);
    }
    return {true, path, boost::none};
  }
  else
  {
    std::vector<Vertex> S;
    AugDigraph::vertex_iterator v_it, v_end;
    for (tie(v_it, v_end) = boost::vertices(d_hat);
         v_it != v_end; v_it++)
    {
      if (reached[*v_it])
        S.push_back(*v_it);
    }

    return {false, boost::none, S};
  }
};


void edmonds_karp(FlowProblem &fp)
{
  // TODO: build augmented digraph first, pass that to build_res_digraph

  int t = 0;
  while (true)
  {
    // 1. Compute residual digraph of D, d_hat
    // AugDigraph d_hat = build_res_digraph(fp.a_d);

    std::tuple<
        bool,
        boost::optional<std::vector<Arc>>,
        boost::optional<std::vector<Vertex>>>
        ret = find_min_path(d_hat, fp.source, fp.sink);
    if (std::get<bool>(ret)) // source-sink path exists
    {
      auto p_ret = std::get<1>(ret);
      std::vector<Arc> p = p_ret.value();
      //  3.2 Get eps = min(res_c(arc) for arc in P)
      int eps = INT_MAX;
      for (auto a : p)
      {
        if (d_hat[a].res_capacity < eps)
        {
          eps = d_hat[a].res_capacity;
        }
      }

      //  3.3 f_base += eps * path_flow
      // for (auto a : p)
      // {
      //   if (d_hat[a].direction == FORWARD)
      //     fp.d[d_hat[a].orig_arc].flow += eps;
      //   else
      //     fp.d[d_hat[a].orig_arc].flow -= eps;
      // }

      // TODO: print res_digraph
    }
    else
    {
      auto s_ret = std::get<2>(ret);
      std::vector<Vertex> S = s_ret.value();

      std::cout << 1 << " ";
      int val_f = 0;

      Digraph::edge_iterator e_it, e_end;
      // TODO: how to get val_f? keep list of arcs that reach sink,
      // probably warrants a sepparate function
      // for (tie(e_it, e_end) = boost::in_edges(fp.sink, fp.d);
      //      e_it != e_end; e_it++)
      // {
      // }

      std::cout << val_f << " " << S.size();
      for (auto v : S)
        std::cout << " " << v + 1;

      std::cout << std::endl;
      return;
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

  edmonds_karp(flow_problem);

  return EXIT_SUCCESS;
}