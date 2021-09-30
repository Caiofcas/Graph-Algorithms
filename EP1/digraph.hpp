#ifndef ARB_H
#define ARB_H

#include <array>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS> Digraph;

typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;

#endif // #ifndef ARB_H
