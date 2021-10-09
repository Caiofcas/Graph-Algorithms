/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "astg.hpp"


void describe_digraph(Digraph dig){
    std::cout << boost::num_vertices(dig) << " ";
    std::cout << boost::num_edges(dig) << std::endl;

    // print all edges
    Digraph::edge_iterator e_it, e_it_end;
    int n_variables = boost::num_vertices(dig)/2;
    // std::cout << "n_variables: " << n_variables << std::endl;

    for(std::tie(e_it, e_it_end) = boost::edges(dig);
        e_it != e_it_end; e_it++){

        std::cout << vertex2literal((*e_it).m_source, n_variables);
        std::cout << " " << vertex2literal((*e_it).m_target, n_variables);
        std::cout << std::endl;
    };

}


int main(int argc, char** argv)
{
    int d, n_variables, n_clauses;
    std::cin >> d >> n_variables >> n_clauses;

    // std::cout << "Read d: " << d << std::endl;
    // std::cout << "Read n (n_variables): " <<  n_variables << std::endl;
    // std::cout << "Read m (n_clauses): " <<  n_clauses << std::endl;
    
    //read_clauses
    Digraph dig(2*n_variables); // One vertex for each literal
   
    // std::cout << "Created digraph with |V|: "<< boost::num_vertices(dig);
    // std::cout << std::endl;

    while(n_clauses--){
        int a, b; 
        
        std::cin >> a >> b;
        // std::cout << "Read clause: " << a << " "<< b << std::endl;

        boost::add_edge(
            literal2vertex(-a, n_variables),
            literal2vertex(b, n_variables),
            dig);

        // std::cout << "Adding edge: ";
        // std::cout << literal2vertex(-a, n_variables) << " (" << -a << ")";
        // std::cout << " <-> ";
        // std::cout << literal2vertex(b, n_variables) << " (" << b << ")";
        // std::cout << std::endl;

        boost::add_edge(
            literal2vertex(-b, n_variables),
            literal2vertex(a, n_variables),
            dig);

        // std::cout << "Adding edge: ";
        // std::cout << literal2vertex(-b, n_variables) << " (" << -b << ")";
        // std::cout << " <-> ";
        // std::cout << literal2vertex(a, n_variables) << " (" << a << ")";
        // std::cout << std::endl;

        // std::cout << "|V|: " << boost::num_vertices(dig);
        // std::cout << " |A|: " << boost::num_edges(dig);
        // std::cout << std::endl;

    }

    d = 2;
    switch (d)
    {
    case 0:
        std::cout << "Debbuging level 0 not implemented yet." << std::endl;
        break;
    
    case 1:
        std::cout << "Debbuging level 1 not implemented yet." << std::endl;
        break;
    
    case 2:
        describe_digraph(dig);
        break;
    }
}
