#include "astg.hpp"

Vertex index2vertex(int i, int n){
    // Will consider:
    //   [0,n-1] "normal literals"
    //   [n,2n-1] "negated literals"
    // to access  x_i -> vertex i-1
    // to_access !x_i -> vertex i-1(+n) 
    Vertex r;

    if (i < 0) r = (-1*i) -1 + n;
    else r = i-1;

    return r;
}
