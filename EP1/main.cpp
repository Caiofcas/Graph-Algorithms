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
    int time, n;
    std::vector<int> color;
    std::vector<int> discovery_time;
    std::vector<int> finish_time;
    std::vector<int> lowlink;
    std::vector<Vertex> parent;
    DFS(int n_vert) {
        n = n_vert;
        time = 0;
        color = std::vector<int>(n, WHITE);
        discovery_time = std::vector<int>(n, 0);
        finish_time = std::vector<int>(n, 0);
        parent = std::vector<Vertex>(n);
        lowlink = std::vector<int>(n, 0);
    };

    
    /* ==========================================
     *      Strong Component Labeling Methods
     * ========================================== */
    
    //For each vertex, print each strong component it belongs to
    static std::vector<int> label_vertexes_by_strong_comp(Digraph* dig){
        int n_vert, nscc; 
        n_vert = boost::num_vertices(*dig);
        
        // 1: Find strong components (Tarjan)
        
        // 1.1: Setup
        DFS dfs(n_vert);
        std::vector<int> sc_labeling(n_vert, -1);
        std::vector<bool> in_stack(n_vert, false);
        std::stack<Vertex> stack;
        Digraph::vertex_iterator v_it, v_it_end;

        nscc = 0;

        // 1.2: Main Loop
        std::tie(v_it, v_it_end) = boost::vertices(*dig);
        for(; v_it != v_it_end; v_it++){
            if(dfs.color[*v_it] == DFS::WHITE){
                dfs.visit(
                    *v_it,
                    &stack, &in_stack,
                    &nscc, &sc_labeling,
                    dig
                );
            }
        }

        return sc_labeling;
    }
    /* ==========================================
     *            Performing DFS Methods
     * ========================================== */
    // Visit u
    void discover(Vertex u){
        time++;
        discovery_time[u] = time;
        lowlink[u] = time;
        color[u] = GRAY;
        // std::cout << "Discovered vertex: " << vertex2literal(u,n/2);
        // std::cout << " [" << time << "]" << std::endl;
    }

    // Finish visiting u
    void finish(Vertex u){
        time++;
        finish_time[u] = time;
        color[u] = BLACK;
        // std::cout << "Left vertex: " << vertex2literal(u, n/2);
        // std::cout << " [" << time << "]" << std::endl;
    }

    // Main DFS method
    void visit(
            Vertex u,
            std::stack<Vertex>* stack,
            std::vector<bool>* in_stack,
            int* nscc,
            std::vector<int>* sc_labeling,
            Digraph* dig)
        {
        
        this->discover(u);
        (*stack).push(u);
        (*in_stack)[u] = true;
        // std::cout << "Put vertex " << vertex2literal(u,dfs->n/2);
        // std::cout << " in stack (size:" << stack->size() << ")" << std::endl;

        // Visit adjacent vertices to u
        Digraph::adjacency_iterator v_it, v_it_end;
        std::tie(v_it,v_it_end) = boost::adjacent_vertices(u, *dig);

        for(; v_it != v_it_end; v_it++){
            if(this->color[*v_it] == DFS::WHITE){
                this->parent[*v_it] = u;
                this->visit(
                    *v_it,
                    stack, in_stack,
                    nscc,
                    sc_labeling,
                    dig
                );
                // update lowpoint
                this->update_lp(u, *v_it, true);
            }
            // (u,v) is back or cross arc AND v is in stack 
            else if (!this->is_tree_arc(u, *v_it) && (*in_stack)[*v_it]){
                this->update_lp(u, *v_it, false);
            }
        }

        this->finish(u);

        //identify strong components
        if (this->is_base_vertex(u)){
            // std::cout << vertex2literal(u,dfs->n/2) << " is a base vertex!" << std::endl;
            Vertex v;
            (*nscc)++;
            do
            {
                v = (*stack).top();
                (*stack).pop();
                (*in_stack)[v] = false;
                (*sc_labeling)[v] = *nscc;
                // std::cout << "Assigned SC (" << *nscc << ") to vertex: ";
                // std::cout << vertex2literal(u,dfs->n/2) << std::endl;
            } while (v != u);
        }
    }

    /* ==========================================
     *            Helper Methods
     * ========================================== */
    // Update lowlink if smaller candidate    
    void update_lp(Vertex u, Vertex v, bool use_lp){
        int candidate_val;

        if(use_lp) candidate_val = lowlink[v];
        else candidate_val = discovery_time[v];
        
        if(lowlink[u] > candidate_val) 
            lowlink[u] = candidate_val;
     
        // std::cout << "lowlink of vertex " << vertex2literal(u, n/2);
        // std::cout << " is now " << lowlink[u] << std::endl;
    }

    bool is_tree_arc(Vertex u, Vertex v){
        return (discovery_time[u] < discovery_time[v]) \
             & (finish_time[v] < finish_time[u]);
    }

    bool is_base_vertex(Vertex u){
        // std::cout << "is Vertex " << vertex2literal(u, n/2) << " a base value? ";
        // std::cout << "d: " << discovery_time[u] << " ll: " << lowlink[u];
        // std::cout << " Answer: " << (discovery_time[u] == lowlink[u]) << std::endl;
        return discovery_time[u] == lowlink[u];
    }
};




int main(int argc, char** argv)
{
    int d, n_variables, n_clauses;
    Digraph::vertex_iterator v_it, v_it_end;
    std::vector<int> sc_labeling;    

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

    // d = 1;
    if (d==2) describe_digraph(dig);
    else {
        sc_labeling = DFS::label_vertexes_by_strong_comp(&dig);
        if (d==1){
            // Iterate through vertexes and print which strong 
            // component it belongs to.
            std::tie(v_it, v_it_end) = boost::vertices(dig);
            for(; v_it != v_it_end; v_it++){
                std::cout << sc_labeling[*v_it] << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Debbuging level 0 not implemented yet." << std::endl;
        }
    }
}
