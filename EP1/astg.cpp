#include "astg.hpp"

Vertex literal2vertex(int l, int n){
    // l: literal to be converted
    // n: number of variables
    // Will consider:
    //   [0,n-1] "normal literals"
    //   [n,2n-1] "negated literals"
    // to access  x_l -> vertex l-1
    // to_access !x_l -> vertex l-1(+n) 
    Vertex r;

    if (l < 0) r = (-1*l) -1 + n;
    else r = l-1;

    return r;
}
