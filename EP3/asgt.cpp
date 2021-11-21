#include "asgt.h"

#include <tuple>
#include <vector>
#include <iomanip>              // for std::setw

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

using boost::adjacent_vertices;
using boost::out_edges;
using boost::vertices;
using std::vector;
using std::cout;
using std::endl;
using std::tie;


static size_t count_digits(size_t n)
{
  return std::to_string(n).size();
}

Digraph build_digraph(const Digraph& market)
{
  cout << "=============================================" << endl;
  cout << "            Building Aux Digraph" << endl;
  cout << "=============================================" << endl;

  Digraph digraph(boost::num_vertices(market));
  Digraph::vertex_iterator v_it, v_end;
  Arc aux_arc, orig_arc;
  Vertex u, v;

  // Iteration stuff
  for(tie(v_it, v_end) = vertices(market); v_it != v_end; v_it++){
    cout << "Iterating through vertex " << *v_it+1 << endl;

    for(auto e_it = out_edges(*v_it, market);
      e_it.first != e_it.second; e_it.first++){
        
        // Give some nicer names to variables
        orig_arc = *(e_it.first);
        u = boost::target(orig_arc, market);
        v = *(v_it);

        cout << "Adding arc (" << v+1 << ")(" << u+1 << ")" << endl;

        // Create arc in aux digraph
        aux_arc = boost::add_edge(v, u, digraph).first;
        
        // Add new arc cost
        digraph[aux_arc].cost = -log(market[orig_arc].cost);
        cout << "Arc cost: " << -log(market[orig_arc].cost) << endl;
    }
  }

  cout << "=============================================" << endl;
  cout << "        Finished Building Aux Digraph" << endl;
  cout << "=============================================" << endl;

  return digraph;
}

int bellman_ford(
  Vertex s,
  vector<double> &d,
  vector<Walk*> &W,
  Digraph& digraph,
  int n, int width
  ){

  cout << "=============================================" << endl;
  cout << "          Entering bellman_ford" << endl;
  cout << "=============================================" << endl;

  int l;
  vector<double> old_d(n, INFINITY);
  vector<Walk*> old_W(n, NULL);
  Vertex u, v;
  Walk aux(digraph, s);
  Arc aux_arc;
  Digraph::vertex_iterator u_it, u_end;
  Digraph::adjacency_iterator v_it, v_end; 

  // Set s (starting value) values
  d[s] = old_d[s] = 0;
  W[s] = new Walk(digraph, s);

  // We iterate on u, not on v, due to how adjacent_vertices work
  for(l=1; l <= n; l++) {
    for(tie(u_it, u_end) = vertices(digraph); u_it != u_end; u_it++){
      cout << "Iterating on Path vertex " << *u_it+1 << endl;

      cout << "Will now iterate through vertices that are reached by ";
      cout << *u_it+1 << endl;

      for(auto e_it = out_edges(*u_it, digraph);
      e_it.first != e_it.second; e_it.first++){

        // Give some nicer names to variables
        aux_arc = *(e_it.first);
        u = *u_it;
        v = boost::target(aux_arc, digraph);
        cout << "Iterating on vertex " << v+1 << endl; 

        cout << "Edge (" << u+1 << ")(" << v+1 << ") has cost ";
        cout << digraph[aux_arc].cost << endl;

        if (d[v] > old_d[u] + digraph[aux_arc].cost){
          d[v] = old_d[u] + digraph[aux_arc].cost;
          cout << "Updated d[" << v+1 << "]" << endl;

          if(old_W[u] == NULL)
            W[v] = new Walk(digraph, u);
          else
            W[v] = new Walk(*old_W[u]);

          bool extend_suceeded = W[v]->extend(aux_arc);
          cout << "Updated W[" << v+1 << "] " << extend_suceeded << endl;
          cout << "W[" << v+1 << "]: "<< W[v] << endl;
        }
        cout << "d[" << v+1 << "]: " << d[v] << endl;
      }
    }

    cout << "Calculated distances ("<< l <<"):" << endl;
    for(tie(u_it, u_end) = vertices(digraph); u_it != u_end; u_it++){
      cout << "  d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
      cout << "old_d[" << std::setw(width) << *u_it+1 << "]: " << old_d[*u_it] << endl;
      cout << "  W[" << *u_it+1 << "]: " << W[*u_it] << endl;
    }

    // Time step (if l == n no need to perform it, keep last iteration values around)
    if (l == n) break;

    for(tie(u_it, u_end) = vertices(digraph); u_it != u_end; u_it++){
          old_d[*u_it] = d[*u_it];
          old_W[*u_it] = W[*u_it];
    }
  }

  cout << "Calculated distances (End BF run):" << endl;
  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
    cout << "d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
    cout << "old_d[" << std::setw(width) << *u_it+1 << "]: " << old_d[*u_it] << endl;
  }

  cout << "Verifying if cycle has been found" << endl;
  for(int i = 0; i < n; i++){
    if (old_d[i] != d[i]){
      cout << "Negative cycle on vertex " << i+1 << " with cost ";
      double cost = 0;
      vector<Arc> walk_arcs = W[i]->arcs();
      for(Arc a_it : W[i]->arcs())
        cost += digraph[a_it].cost;
      cout << cost << endl;

      cout << "=============================================" << endl;
      cout << "        Exiting bellman_ford (cycle)" << endl;
      cout << "=============================================" << endl;
      return i;
    }
  }

  cout << "=============================================" << endl;
  cout << "        Exiting bellman_ford (no cycle)" << endl;
  cout << "=============================================" << endl;
  return -1;
}


std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  cout << "=============================================" << endl;
  cout << "         Entered has_negative_cycle" << endl;
  cout << "=============================================" << endl;


  int n = num_vertices(digraph), width, cycle_i;
  Vertex u;
  vector<double> d(n, INFINITY);
  vector<Walk*> W(n, NULL);
  Digraph::vertex_iterator u_it, u_end;

  width = count_digits(n);
  // Set s as first vertex (0)

  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
    u = *u_it;
    if (d[u] == INFINITY){
      cycle_i = bellman_ford(u, d, W, digraph, n, width);
      if (cycle_i >= 0){
        cout << "Building cycle out of Walk in vertex " << cycle_i+1 << endl;
        NegativeCycle c(*W[cycle_i]);
        cout << "=============================================" << endl;
        cout << "       Exiting has_negative_cycle (true)" << endl;
        cout << "=============================================" << endl;

        return {true, c, boost::none};
      }
    }
  }

  cout << "Calculated distances (Final):" << endl;
  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
    cout << "d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
  }

  cout << "=============================================" << endl;
  cout << "       Exiting has_negative_cycle (false)" << endl;
  cout << "=============================================" << endl;

  return {false, boost::none, FeasiblePotential(digraph, d)};
}

Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market)
{
  /* bogus code */
  const Arc& b0 = *(out_edges(0, market).first);
  const Arc& b1 = *(out_edges(1, market).first);

  Walk w(market, 0);
  w.extend(b0);
  w.extend(b1);

  // encourage RVO
  return Loophole(w);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market)
{
  int n = num_vertices(market);
  vector<double> z(n);
  vector<double> y = feaspot.potential();

  for (int i = 0; i < n; i++)
    z[i] = exp(-y[i]);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
