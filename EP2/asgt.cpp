#include "asgt.h"
#include <stack>

class DFS {
public:
    // Colors for DFS
    static const short WHITE = 0;
    static const short GRAY = 1;
    static const short BLACK = 2;

    // Instance data
    int time, n, next;
    Graph* gr;
    std::vector<int> color;
    std::vector<int> discovery_time;
    std::vector<int> finish_time;
    std::vector<int> lowlink;
    std::vector<Vertex> top_order;
    std::vector<Vertex> parent;

    DFS(Graph* gr) {
        this->n = boost::num_vertices(*gr);
        this->gr = gr;
        this->next = this->time = 0;
        this->color = std::vector<int>(this->n, WHITE);
        this->discovery_time = std::vector<int>(this->n, 0);
        this->finish_time = std::vector<int>(this->n, 0);
        this->lowlink = std::vector<int>(this->n, 0);
        this->parent = std::vector<Vertex>(this->n);
        this->top_order = std::vector<Vertex>(this->n);
    };

    /* ====================================================
     *                 Strong Component Labeling
     * ==================================================== */
    
    std::vector<int> label_vertexes_by_strong_comp(){
        
        std::vector<int> sc_labeling(this->n, -1);
        std::vector<bool> in_stack(this->n, false);
        std::stack<Vertex> stack;
        Graph::vertex_iterator v_it, v_it_end;
        int nscc = 0;

        std::tie(v_it, v_it_end) = boost::vertices(*(this->gr));
        for(; v_it != v_it_end; v_it++){
            if(this->color[*v_it] == DFS::WHITE){
                this->visit(*v_it,
                            &stack, &in_stack,
                            &nscc, &sc_labeling);
            }
        }

        return sc_labeling;
    }

    /* ====================================================
     *                 Performing DFS Methods
     * ==================================================== */

    // Visit u
    void discover(Vertex u){
        this->time++;
        this->discovery_time[u] = this->time;
        this->lowlink[u] = this->time;
        this->color[u] = DFS::GRAY;
    }

    // Finish visiting u
    void finish(Vertex u){
        this->time++;
        this->finish_time[u] = time;
        this->color[u] = DFS::BLACK;
        this->top_order[u] = this->time;
    }

    // Main DFS method
    void visit(
        Vertex u,
        std::stack<Vertex>* stack,
        std::vector<bool>* in_stack,
        int* nscc,
        std::vector<int>* sc_labeling)
        {
        
        this->discover(u);
        (*stack).push(u);
        (*in_stack)[u] = true;

        // Visit adjacent vertices to u
        Graph::adjacency_iterator v_it, v_it_end;
        std::tie(v_it,v_it_end) = boost::adjacent_vertices(u, *(this->gr));

        for(; v_it != v_it_end; v_it++){
            if(this->color[*v_it] == DFS::WHITE){
                this->parent[*v_it] = u;
                this->visit(
                    *v_it,
                    stack, 
                    in_stack,
                    nscc, 
                    sc_labeling
                );
                // update lowlink
                this->update_ll(u, *v_it, true);
            }
            // (u,v) is back or cross arc AND v is in stack 
            else if (!this->is_tree_arc(u, *v_it) && (*in_stack)[*v_it]){
                this->update_ll(u, *v_it, false);
            }
        }

        this->finish(u);

        //identify strong components
        if (this->is_base_vertex(u)){
            Vertex v;
            (*nscc)++;
            do {
                v = (*stack).top();
                (*stack).pop();
                (*in_stack)[v] = false;
                (*sc_labeling)[v] = *nscc;
            } while (v != u);
        }
    }
    
    /* ====================================================
     *                   Helper Methods
     * ==================================================== */

    // Update lowlink if smaller candidate    
    void update_ll(Vertex u, Vertex v, bool use_lp){
        int candidate_val;

        if(use_lp) candidate_val = lowlink[v];
        else candidate_val = discovery_time[v];
        
        if(lowlink[u] > candidate_val) lowlink[u] = candidate_val;
    }

    bool is_tree_arc(Vertex u, Vertex v){
        return (discovery_time[u] < discovery_time[v]) \
             & (finish_time[v] < finish_time[u]);
    }

    bool is_base_vertex(Vertex u){
        return discovery_time[u] == lowlink[u];
    }
};

void find_cut_vertexes(Graph &g)
{
  for (const auto& vertex : boost::make_iterator_range(boost::vertices(g))) {
    g[vertex].cutvertex = false;
  }

}

void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges)
{
  /* fill everything with dummy values */
  if (fill_cutvxs)
  {
    find_cut_vertexes(g);
  };
  
  for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
    g[edge].bcc = 0;
    g[edge].bridge = false;
  }
}
