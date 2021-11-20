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

// TODO need to check for neg cycles on each run
void
bellman_ford(
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
  Arc aux_arc;
  Digraph::vertex_iterator u_it, u_end;
  Digraph::adjacency_iterator v_it, v_end; 

  // Set s (starting value) values
  d[s] = old_d[s] = 0;
  // W[s] = <s>

  // We iterate on u, not on v, due to how adjacent_vertices work
  for(l=0; l < n; l++) {
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
          // W[v] = old_W[u] + v;
          // TODO: walk manipulation
          cout << "Updated d[" << v+1 << "]" << endl;
        }
        cout << "d[" << v+1 << "]: " << d[v] << endl;
      }
    }

    // Time step (if l == n no need to perform it, keep last iteration values around)
    if (l == n) break;

    for(tie(u_it, u_end) = vertices(digraph); u_it != u_end; u_it++){
          old_d[*u_it] = d[*u_it];
          old_W[*u_it] = W[*u_it];
    }

    cout << "Calculated distances ("<< l <<"):" << endl;
    for(tie(u_it, u_end) = vertices(digraph); u_it != u_end; u_it++)
      cout << "  d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;

  }

  cout << "Calculated distances (End BF run):" << endl;
  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
    cout << "d[" << std::setw(width) << *u_it+1 << "]: " << d[*u_it] << endl;
  }

  cout << "=============================================" << endl;
  cout << "          Exiting bellman_ford" << endl;
  cout << "=============================================" << endl;
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
  // MARCEL ENDS


  int n = num_vertices(digraph), width;
  Vertex u;
  vector<double> d(n, INFINITY);
  vector<Walk*> W(n, NULL);
  Digraph::vertex_iterator u_it, u_end;

  width = count_digits(n);
  // Set s as first vertex (0)

  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
    u = *u_it;
    if (d[u] == INFINITY)
      bellman_ford(u, d, W, digraph, n, width);
  }

  cout << "Calculated distances (Final):" << endl;
  for(tie(u_it, u_end) = vertices(digraph);
        u_it != u_end; u_it++){
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
  int n = num_vertices(market);
  vector<double> z(n);
  vector<double> y = feaspot.potential();

  for (int i = 0; i < n; i++)
    z[i] = exp(-y[i]);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
