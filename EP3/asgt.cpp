#include "asgt.h"

#include <utility>              // for std::get
#include <tuple>
#include <vector>
#include <iomanip>              // for std::setw

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

using boost::out_edges;
using std::vector;
using std::cout;
using std::endl;


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
  Digraph::vertex_iterator v_it, v_it_end;
  Arc aux_arc, orig_arc;
  Vertex u, v;

  // Iteration stuff
  std::tie(v_it, v_it_end) = boost::vertices(market);
  for(; v_it != v_it_end; v_it++){
    cout << "Iterating through vertex " << *v_it+1 << endl;
    auto u_it = boost::adjacent_vertices(*v_it, market);
    for(; u_it.first != u_it.second; u_it.first++){
        
        // Give some nicer names to variables
        u = *(u_it.first); v = *(v_it);

        cout << "Adding arc (" << v+1 << ")(" << u+1 << ")" << endl;

        // Create arc in aux digraph
        aux_arc = boost::add_edge(v, u, digraph).first;
        
        // Get original arv cost
        orig_arc = boost::edge(v, u, market).first;
        
        // Add new arc cost
        digraph[aux_arc].cost = -log10(market[orig_arc].cost);
        cout << "Arc cost: " << -log10(market[orig_arc].cost) << endl;
    }
  }

  cout << "=============================================" << endl;
  cout << "        Finished Building Aux Digraph" << endl;
  cout << "=============================================" << endl;

  return digraph;
}

// TODO need to check for neg cycles on each run
void
bellman_ford(
  Vertex s,
  vector<double> &d,
  vector<Walk*> &W,
  Digraph& digraph,
  int width
  ){

  int l, n = num_vertices(digraph);
  vector<double> old_d(n, INFINITY);
  vector<Walk*> old_W(n, NULL);
  Vertex u, v;
  Digraph::vertex_iterator u_it, u_it_end;
  Digraph::adjacency_iterator v_it, v_it_end; 


  // Set s (starting value) values
  d[s] = old_d[s] = 0;
  // W[s] = <s>

  // We iterate on u, not on v, due to how adjacent_vertices work
  for(l=0; l < n; l++) {
    for(std::tie(u_it, u_it_end) = boost::vertices(digraph);
        u_it != u_it_end; u_it++){
      u = *u_it;
      cout << "Iterating on vertex " << u+1 << endl;

      cout << "Will now iterate through vertices that are reached by " << u+1 << endl;

      for(std::tie(v_it, v_it_end) = boost::adjacent_vertices(u, digraph);
        v_it != v_it_end; v_it++){

        v = (*v_it);
        cout << "Iterating on Path vertex " << v+1 << endl; 

        auto aux_arc = boost::edge(u, v, digraph).first;

        cout << "Edge " << u+1 << v+1 << " has cost " << digraph[aux_arc].cost << endl;
        if (d[v] > old_d[u] + digraph[aux_arc].cost){
          d[v] = old_d[u] + digraph[aux_arc].cost;
          // W[v] = old_W[u] + v;
          // TODO: walk manipulation
          cout << "Updated d[" << v+1 << "]" << endl;
        }
        cout << "d[" << v+1 << "]: " << d[v] << endl;
      }
    }
    // Time step (if l == n no need to perform it, keep last iteration values around)

    if (l == n) break;
    for(std::tie(u_it, u_it_end) = boost::vertices(digraph);
        u_it != u_it_end; u_it++){
          u = *u_it;
          old_d[u] = d[u];
          old_W[u] = W[u];
    }
    cout << "Calculated distances ("<< l <<"):" << endl;
    for(std::tie(u_it, u_it_end) = boost::vertices(digraph);
        u_it != u_it_end; u_it++){
      cout << "  d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
    }

  }

  cout << "Calculated distances (End BF run):" << endl;
  for(std::tie(u_it, u_it_end) = boost::vertices(digraph);
        u_it != u_it_end; u_it++){
    cout << "d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
  }

}


std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  cout << "=============================================" << endl;
  cout << "         Entered has_negative_cycle" << endl;
  cout << "=============================================" << endl;
  // MARCEL STARTS
  /* const Arc& a0 = *(out_edges(0, digraph).first);
  // const Arc& a1 = *(out_edges(1, digraph).first);
  // Walk walk(digraph, 0);
  // walk.extend(a0);
  // walk.extend(a1); */
  /* Replace `NegativeCycle(walk)` with `boost::none` in the next
   * command to trigger "negative cycle reported but not computed".
   * Comment the whole `return` and uncomment the remaining lines to
   * exercise construction of a feasible potential. */
  // encourage RVO
  // return {true, NegativeCycle(walk), boost::none};
  /* Replace `FeasiblePotential(digraph, y)` with `boost::none` in the
   * next command to trigger "feasible potential reported but not
   * computed". */
  // MARCEL ENDS


  // PROBLEM: disconnected components will not be calculated
  int n = num_vertices(digraph), width;
  vector<double> d(n, INFINITY);
  vector<Walk*> W(n, NULL);
  Digraph::vertex_iterator u_it, u_it_end;
  Digraph::adjacency_iterator v_it, v_it_end; 

  width = count_digits(n);
  // Set s as first vertex (0)
  bellman_ford(0, d, W, digraph, width);

  cout << "Calculated distances (Final):" << endl;
  for(std::tie(u_it, u_it_end) = boost::vertices(digraph);
        u_it != u_it_end; u_it++){
    cout << "d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
  }

  cout << "=============================================" << endl;
  cout << "          Exiting has_negative_cycle" << endl;
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
  vector<double> z(num_vertices(market), 1.0);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
