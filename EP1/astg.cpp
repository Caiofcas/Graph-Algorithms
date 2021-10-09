#include "astg.hpp"

// Will consider:
//   [0,n-1] "normal literals"
//   [n,2n-1] "negated literals"
// to access  x_l <-> vertex l-1
// to_access !x_l <-> vertex l-1(+n) 

Vertex literal2vertex(int l, int n){
    // l: literal to be converted
    // n: number of variables ((|V|)
    Vertex r;

    if (l < 0) r = (-1*l) -1 + n;
    else r = l-1;

    return r;
}

int vertex2literal(Vertex v, int n){
    // v: vertex to be converted
    // n: number of variables ((|V|)
    int r;

    if (v >= static_cast<unsigned int>(n)) r = -(v - n + 1);
    else r = v + 1;

    return r;
}