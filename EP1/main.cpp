/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "astg.hpp"


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

    switch (d)
    {
    case 0:
        printf("Debbuging level 0 not implemented yet.\n");
        break;
    
    case 1:
        printf("Debbuging level 1 not implemented yet.\n");
        break;
    
    case 2:
        printf("Debbuging level 2 not implemented yet.\n");
        break;
    }
}
