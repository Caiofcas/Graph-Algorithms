#include <iostream>
#include "astg.hpp"
#include <stack>



/* ==============================================
 *              Debugging level 2
 * ============================================== */
// Print |V|, then |A|, then all arcs in digraph
void describe_digraph(Digraph dig){
    Digraph::edge_iterator e_it, e_it_end;
    int n_vars;

    std::cout << boost::num_vertices(dig) << " ";
    std::cout << boost::num_edges(dig) << std::endl;

    std::tie(e_it, e_it_end) = boost::edges(dig);
    n_vars = boost::num_vertices(dig)/2;

    for(;e_it != e_it_end; e_it++){
        std::cout << vertex2literal((*e_it).m_source, n_vars);
        std::cout << " " << vertex2literal((*e_it).m_target, n_vars);
        std::cout << std::endl;
    };
}


/* ==============================================
 *              Debugging level 1
 * ============================================== */

class DFS {
public:
    // Colors for DFS
    static const short WHITE = 0;
    static const short GRAY = 1;
    static const short BLACK = 2;

    // Instance data
    int time;
    std::vector<int> color;
    std::vector<int> discovery_time;
    std::vector<int> finish_time;
    std::vector<int> lowpoint;
    std::vector<Vertex> parent;
    DFS(int n) {
        color = std::vector<int>(n, WHITE);
        discovery_time = std::vector<int>(n, 0);
        finish_time = std::vector<int>(n, 0);
        parent = std::vector<Vertex>(n);
        lowpoint = std::vector<int>(n, 0);
    };

    // Visit u
    void discover(Vertex u){
        time++;
        discovery_time[u] = time;
        lowpoint[u] = time;
        color[u] = GRAY;
    }

    // Finish visiting u
    void finish(Vertex u){
        time++;
        finish_time[u] = time;
        color[u] = BLACK;
    }
    
    void update_lp(Vertex u, Vertex v, bool use_lp){
        int candidate_val;

        if(use_lp) candidate_val = lowpoint[v];
        else candidate_val = discovery_time[v];
        
        if(lowpoint[u] < candidate_val) 
            lowpoint[u] = candidate_val;
    }
};


bool is_back_or_cross_arc(Vertex u, Vertex v, Digraph* dig){
    return false;
}

bool is_base_vertex(Vertex u){
    return false;
}


void dfs_visit(
        Vertex u,
        DFS* dfs,
        std::stack<Vertex>* stack,
        int* nscc,
        std::vector<int>* sc_labeling,
        Digraph* dig
        ){
    
    dfs->discover(u);
    (*stack).push(u);

    // Visit adjacent vertices to u
    Digraph::adjacency_iterator v_it, v_it_end;
    std::tie(v_it,v_it_end) = boost::adjacent_vertices(u,*dig);

    for(; v_it!=v_it_end; v_it++){
        if(dfs->color[*v_it] == DFS::WHITE){
            dfs->parent[*v_it] = u;
            dfs_visit(
                *v_it,
                dfs,
                stack,
                nscc,
                sc_labeling,
                dig
            );
            // update lowpoint
            dfs->update_lp(u, *v_it, true);
        }
        else if (is_back_or_cross_arc(u, *v_it, dig)){
            dfs->update_lp(u, *v_it, false);
        }
    }

    dfs->finish(u);

    //identify strong components
    if (is_base_vertex(u)){
        Vertex v;
        (*nscc)++;
        do
        {
            v = (*stack).top();
            (*stack).pop();
            (*sc_labeling)[v] = *nscc;
        } while (v != u);
    }
}


//For each vertex, print each strong component it belongs to
void label_vertexes_by_strong_comp(Digraph* dig){
    int n_vert, nscc; 
    n_vert = boost::num_vertices(*dig);
    
    // 1: Find strong components (Tarjan)
    
    // 1.1: Setup
    DFS dfs(n_vert);
    std::vector<int> sc_labeling(n_vert, -1);
    std::stack<Vertex> stack;
    Digraph::vertex_iterator v_it, v_it_end;

    nscc = 0;

    // 1.2: Main Loop
    std::tie(v_it, v_it_end) = boost::vertices(*dig);
    for(; v_it != v_it_end; v_it++){
        if(dfs.color[*v_it] == DFS::WHITE){
            dfs_visit(
                *v_it,
                &dfs,
                &stack,
                &nscc,
                &sc_labeling,
                dig
            );
        }
    }

    //Iterate through vertexes and print which strong component it belongs to.
    std::tie(v_it, v_it_end) = boost::vertices(*dig);
    for(; v_it != v_it_end; v_it++){
        std::cout << sc_labeling[*v_it] << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    int d, n_variables, n_clauses;
    std::cin >> d >> n_variables >> n_clauses;

    // std::cout << "Read d: " << d << std::endl;
    // std::cout << "Read n (n_variables): " <<  n_variables << std::endl;
    // std::cout << "Read m (n_clauses): " <<  n_clauses << std::endl;
    
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

    d = 1;
    switch (d)
    {
    case 0:
        std::cout << "Debbuging level 0 not implemented yet." << std::endl;
        break;
    
    case 1:
        label_vertexes_by_strong_comp(&dig);
        break;
    
    case 2:
        describe_digraph(dig);
        break;
    }
}
