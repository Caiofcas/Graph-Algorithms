/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "digraph.hpp"

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


int main(int argc, char** argv)
{
    int d, n_variables, n_clauses;
    std::cin >> d >> n_variables >> n_clauses;
    printf("Read d: %d\n", d);
    printf("Read n: %d\n", n_variables);
    printf("Read m: %d\n", n_clauses);
    
    //read_clauses
    Digraph dig(2*n_variables); // One vertex for each literal

   
    printf("Created digraph\n");
    while(n_clauses--){
        int a, b; 
        std::cin >> a >> b;
        printf("Read clause: %d %d\n", a, b);
        boost::add_edge(
            index2vertex(-a, 2*n_variables),
            index2vertex(b, 2*n_variables),
            dig);
        boost::add_edge(
            index2vertex(a, 2*n_variables),
            index2vertex(-b, 2*n_variables),
            dig);
    }
}
