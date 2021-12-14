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
#include <boost/graph/copy.hpp>

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
  int capacity = 0, flow = 0;
  short direction = FORWARD;
  int res_capacity = 0;
  int orig_arc_idx = 0;
  int back_arc_idx = 0;
  int fwd_arc_idx = 0;
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
  Digraph a_d;
  Vertex source;
  Vertex sink;
  std::vector<Arc> arc_order;
  std::vector<Arc> aug_arc_order;
};

/*========================================================
  ============== END DIGRAPH DEFINITIONS =================
  ========================================================*/

/*========================================================
  ================ DIGRAPH UTILS =========================
  ========================================================*/

FlowProblem read_flow(std::istream &is)
{
  std::cout << "=========================================" << std::endl;
  std::cout << "entra read_flow" << std::endl;
  std::cout << "=========================================" << std::endl;
  typename boost::graph_traits<Digraph>::vertices_size_type n;
  is >> n;

  FlowProblem fp;
  Vertex source, sink;

  fp.d = Digraph(n);
  fp.a_d = Digraph(n);

  size_t m;
  int i, j;

  is >> m;
  is >> source >> sink;
  source--;
  sink--;

  std::cout << "SOURCE:" << source + 1 << std::endl;
  std::cout << "SINK:" << sink + 1 << std::endl;

  i = 0;
  j = 0;
  while (i < m)
  {
    int u, v;
    is >> u >> v;
    Arc a, a_f, a_b;
    std::tie(a, std::ignore) = boost::add_edge(u - 1, v - 1, fp.d);
    std::tie(a_f, std::ignore) = boost::add_edge(u - 1, v - 1, fp.a_d);
    std::tie(a_b, std::ignore) = boost::add_edge(v - 1, u - 1, fp.a_d);

    is >> fp.d[a].capacity;
    std::cout << "capacity: " << fp.d[a].capacity << std::endl;

    fp.a_d[a_f].direction = FORWARD;
    fp.a_d[a_b].direction = BACKWARD;

    fp.arc_order.push_back(a);
    fp.a_d[a_f].orig_arc_idx = fp.a_d[a_b].orig_arc_idx = i++;

    fp.aug_arc_order.push_back(a_f);
    fp.d[a].fwd_arc_idx = j++;
    fp.aug_arc_order.push_back(a_b);
    fp.d[a].back_arc_idx = j++;

    std::cout << "i: " << i << " j: " << j << std::endl;
  }

  fp.source = source;
  fp.sink = sink;

  std::cout << "=========================================" << std::endl;
  std::cout << "sai read_flow" << std::endl;
  std::cout << "=========================================" << std::endl;

  return fp;
}

int get_flow(Digraph &d, Vertex sink)
{
  int flow = 0;
  Digraph::edge_iterator e_it, e_end;
  for (tie(e_it, e_end) = boost::edges(d); e_it != e_end; e_it++)
  {
    if (boost::target(*e_it, d) == sink)
      flow += d[*e_it].flow;
  }

  return flow;
}

void compute_residual_capacities(
    Digraph *d,
    Digraph *ad,
    std::vector<Arc> *ordering)
{
  std::cout << "=========================================" << std::endl;
  std::cout << "entra compute_residual_capacities" << std::endl;
  std::cout << "=========================================" << std::endl;

  Digraph::edge_iterator e_it, e_end;
  for (tie(e_it, e_end) = boost::edges(*ad); e_it != e_end; e_it++)
  {
    Arc orig_arc = ordering->at((*ad)[*e_it].orig_arc_idx);

    // std::cout << "orig_arc: " << rd[*e_it].orig_arc_idx;
    int res_capacity = 0;
    if ((*ad)[*e_it].direction == FORWARD)
      res_capacity = (*d)[orig_arc].capacity - (*d)[orig_arc].flow;
    else
      res_capacity = (*d)[orig_arc].flow;

    (*ad)[*e_it].res_capacity = res_capacity;
    std::cout << "(" << boost::source(*e_it, *ad) + 1 << " -> ";
    std::cout << boost::target(*e_it, *ad) + 1 << ") res_capacity: ";
    std::cout << (*ad)[*e_it].res_capacity << std::endl;
  }

  std::cout << "=========================================" << std::endl;
  std::cout << "sai compute_residual_capacities" << std::endl;
  std::cout << "=========================================" << std::endl;
}

void build_res_digraph(
    Digraph *d,
    Digraph *ad,
    std::vector<Arc> *ordering,
    Digraph *rd)
{
  std::cout << "=========================================" << std::endl;
  std::cout << "entra build_res_digraph" << std::endl;
  std::cout << "=========================================" << std::endl;

  *rd = Digraph(boost::num_vertices(*ad));

  Digraph::edge_iterator e_it, e_end;
  for (tie(e_it, e_end) = boost::edges(*ad); e_it != e_end; e_it++)
  {
    Arc a = *e_it;
    if ((*ad)[a].res_capacity != 0)
    {
      boost::add_edge(
          boost::source(a, *ad),
          boost::target(a, *ad),
          {
              (*ad)[a].capacity,
              (*ad)[a].flow,
              (*ad)[a].direction,
              (*ad)[a].res_capacity,
              (*ad)[a].orig_arc_idx,
              (*ad)[a].back_arc_idx,
              (*ad)[a].fwd_arc_idx,
          },
          *rd);
    }
  }

  std::cout << "=========================================" << std::endl;
  std::cout << "sai build_res_digraph" << std::endl;
  std::cout << "=========================================" << std::endl;
}

void print_aug_digraph(
    Digraph *d,
    Digraph *aug_d,
    std::vector<Arc> *orig_ordering,
    std::vector<Arc> *aug_ordering)
{
  for (Arc a : *orig_ordering)
  {
    Arc a_b, a_f;
    // std::cout <<"(" << boost::source(a, *d)+1 << " -> ";
    // std::cout << boost::target(a, *d)+1<< ") ";
    // std::cout << "fwd_arc_idx: " << (*d)[a].fwd_arc_idx;
    // std::cout << " back_arc_idx: " << (*d)[a].back_arc_idx << std::endl;
    a_b = aug_ordering->at((*d)[a].back_arc_idx);
    a_f = aug_ordering->at((*d)[a].fwd_arc_idx);

    // std::cout <<"a_f : (" << boost::source(a_f, *aug_d)+1 << " -> ";
    // std::cout << boost::target(a_f, *aug_d)+1<< ") " << (*aug_d)[a_f].res_capacity << std::endl;

    // std::cout <<"a_b : (" << boost::source(a_b, *aug_d)+1 << " -> ";
    // std::cout << boost::target(a_b, *aug_d)+1<< ") " << (*aug_d)[a_b].res_capacity << std::endl;

    std::cout << (*aug_d)[a_f].res_capacity << " ";
    std::cout << (*aug_d)[a_b].res_capacity << std::endl;
  }
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
find_min_path(Digraph &d_hat, Vertex start, Vertex target)
{

  std::cout << "=========================================" << std::endl;
  std::cout << "entra find_min_path" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::cout << "Start: " << start + 1;
  std::cout << " Target: " << target + 1 << std::endl;

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

    std::cout << "Visiting vertex: " << v + 1 << std::endl;

    for (auto a_it = boost::out_edges(v, d_hat);
         a_it.first != a_it.second; a_it.first++)
    {

      Vertex u = boost::target(*a_it.first, d_hat);
      std::cout << "Reached vertex: " << u + 1 << std::endl;
      if (!reached[u])
      {
        std::cout << "First time vertex " << u + 1 << " is reached" << std::endl;
        q.push(u);
        reached[u] = true;
        reached_by[u] = *a_it.first;
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

    std::cout << "=========================================" << std::endl;
    std::cout << "sai find_min_path (true)" << std::endl;
    std::cout << "=========================================" << std::endl;
    return {true, path, boost::none};
  }
  else
  {
    std::vector<Vertex> S;
    Digraph::vertex_iterator v_it, v_end;
    for (tie(v_it, v_end) = boost::vertices(d_hat);
         v_it != v_end; v_it++)
    {
      if (reached[*v_it])
        S.push_back(*v_it);
    }

    std::cout << "=========================================" << std::endl;
    std::cout << "sai find_min_path (false)" << std::endl;
    std::cout << "=========================================" << std::endl;
    return {false, boost::none, S};
  }
};

void edmonds_karp(FlowProblem &fp)
{
  // TODO: build augmented digraph first, pass that to build_res_digraph
  std::cout << "=========================================" << std::endl;
  std::cout << "entra edmonds_karp" << std::endl;
  std::cout << "=========================================" << std::endl;

  int t = 0;
  while (true)
  {
    // 1. Compute residual digraph of D, d_hat
    compute_residual_capacities(&fp.d, &fp.a_d, &fp.arc_order);

    Digraph d_f;
    build_res_digraph(&fp.d, &fp.a_d, &fp.arc_order, &d_f);

    print_aug_digraph(&fp.d, &fp.a_d, &fp.arc_order, &fp.aug_arc_order);
    auto ret = find_min_path(d_f, fp.source, fp.sink);
    if (std::get<bool>(ret)) // source-sink path exists
    {
      auto p_ret = std::get<1>(ret);
      std::vector<Arc> p = p_ret.value();
      //  3.2 Get eps = min(res_c(arc) for arc in P)
      int eps = INT_MAX;
      for (auto a : p)
      {
        std::cout << "(" << boost::source(a, d_f) + 1 << " -> ";
        std::cout << boost::target(a, d_f) + 1 << ") : " << d_f[a].res_capacity << std::endl;
        if (d_f[a].res_capacity < eps)
        {
          eps = d_f[a].res_capacity;
        }
      }

      std::cout << "eps: " << eps << std::endl;
      //  3.3 f_base += eps * path_flow
      for (auto a : p)
      {
        Arc orig_arc = fp.arc_order.at(d_f[a].orig_arc_idx);
        if (d_f[a].direction == FORWARD)
          fp.d[orig_arc].flow += eps;
        else
          fp.d[orig_arc].flow -= eps;
        std::cout << "(" << boost::source(orig_arc, fp.d) + 1 << " -> ";
        std::cout << boost::target(orig_arc, fp.d) + 1 << ") : " << fp.d[orig_arc].flow << std::endl;
      }

      // TODO: rm endl
      std::cout << 0 << " " << eps << " " << p.size() << std::endl;
      for (auto v : p)
        std::cout << " " << (fp.d[v].orig_arc_idx + 1) * d_f[v].direction;
      std::cout << std::endl;
    }
    else
    {
      auto s_ret = std::get<2>(ret);
      std::vector<Vertex> S = s_ret.value();

      int val_f = get_flow(fp.d, fp.sink);

      // TODO: how to get val_f? keep list of arcs that reach sink,
      // probably warrants a sepparate function
      // for (tie(e_it, e_end) = boost::in_edges(fp.sink, fp.d);
      //      e_it != e_end; e_it++)
      // {
      // }

      // TODO: rm endl
      std::cout << 1 << " " << val_f << " " << S.size() << std::endl;
      for (auto v : S)
        std::cout << " " << v + 1;

      std::cout << std::endl;
      return;
    }
    t++;
  }
  std::cout << "=========================================" << std::endl;
  std::cout << "sai edmonds_karp" << std::endl;
  std::cout << "=========================================" << std::endl;
};

/*========================================================
  ================ END DIGRAPH UTILS =====================
  ========================================================*/

int main(int argc, char **argv)
{
  FlowProblem flow_problem{read_flow(std::cin)};

  for (auto a : flow_problem.arc_order)
  {
    std::cout << "arc: " << boost::source(a, flow_problem.d) + 1;
    std::cout << " -> " << boost::target(a, flow_problem.d) + 1;
    std::cout << " (" << flow_problem.d[a].capacity << ")" << std::endl;
  }

  std::cout << "SOURCE:" << flow_problem.source + 1 << std::endl;
  std::cout << "SINK:" << flow_problem.sink + 1 << std::endl;
  edmonds_karp(flow_problem);

  return EXIT_SUCCESS;
}