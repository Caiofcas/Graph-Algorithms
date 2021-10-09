/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "astg.hpp"

void describe_digraph(Digraph dig){
    printf("%ld %ld\n",
        boost::num_vertices(dig), 
        boost::num_edges(dig));
}


int main(int argc, char** argv)
{
    int d, n_variables, n_clauses;
    std::cin >> d >> n_variables >> n_clauses;
    printf("Read d: %d\n", d);
    printf("Read n (n_variables): %d\n", n_variables);
    printf("Read m (n_clauses): %d\n", n_clauses);
    
    //read_clauses
    Digraph dig(2*n_variables); // One vertex for each literal
   
    printf("Created digraph with |V|: %ld\n", boost::num_vertices(dig) );
    while(n_clauses--){
        int a, b; 
        std::cin >> a >> b;
        printf("Read clause: %d %d\n", a, b);
        boost::add_edge(
            literal2vertex(-a, n_variables),
            literal2vertex(b, n_variables),
            dig);
        printf("Adding edge: %ld <-> %ld\n",
            literal2vertex(-a, n_variables),
            literal2vertex(b, n_variables));
        boost::add_edge(
            literal2vertex(a, n_variables),
            literal2vertex(-b, n_variables),
            dig);
        printf("Adding edge: %ld <-> %ld\n",
            literal2vertex(a, n_variables),
            literal2vertex(-b, n_variables));
        printf("|V|: %ld   |A|: %ld\n",
            boost::num_vertices(dig), 
            boost::num_edges(dig));
    }

    d = 2;
    switch (d)
    {
    case 0:
        printf("Debbuging level 0 not implemented yet.\n");
        break;
    
    case 1:
        printf("Debbuging level 1 not implemented yet.\n");
        break;
    
    case 2:
        describe_digraph(dig);
        // printf("Debbuging level 2 not implemented yet.\n");
        break;
    }
}
