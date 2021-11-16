#include "asgt.h"

#include <utility>              // for std::get
#include <tuple>
#include <vector>

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

        cout << "Adding arc " << v+1 << u+1 << endl;

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

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph& digraph)
{
  const Arc& a0 = *(out_edges(0, digraph).first);
  const Arc& a1 = *(out_edges(1, digraph).first);

  Walk walk(digraph, 0);
  walk.extend(a0);
  walk.extend(a1);

  /* Replace `NegativeCycle(walk)` with `boost::none` in the next
   * command to trigger "negative cycle reported but not computed".
   * Comment the whole `return` and uncomment the remaining lines to
   * exercise construction of a feasible potential. */

  // encourage RVO
  return {true, NegativeCycle(walk), boost::none};

  /* Replace `FeasiblePotential(digraph, y)` with `boost::none` in the
   * next command to trigger "feasible potential reported but not
   * computed". */

  // encourage RVO
  vector<double> y(num_vertices(digraph), 0.0);
  return {false, boost::none, FeasiblePotential(digraph, y)};
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
