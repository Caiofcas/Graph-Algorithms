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
    std::vector<int> low;
    std::vector<Vertex> parent;

    DFS(Graph* gr) {
        this->n = boost::num_vertices(*gr);
        this->gr = gr;
        this->next = this->time = 0;
        this->color = std::vector<int>(this->n, WHITE);
        this->discovery_time = std::vector<int>(this->n, 0);
        this->finish_time = std::vector<int>(this->n, 0);
        this->low = std::vector<int>(this->n, 0);
        this->parent = std::vector<Vertex>(this->n);
    };


    /* ====================================================
     *                 Performing DFS Methods
     * ==================================================== */

    // Visit u
    void discover(Vertex u) {
        this->time++;
        this->discovery_time[u] = this->time;
        this->low[u] = this->time;
        this->color[u] = DFS::GRAY;
    }

    // Finish visiting u
    void finish(Vertex u) {
        this->time++;
        this->finish_time[u] = time;
        this->color[u] = DFS::BLACK;
    }

    // Main DFS method
    void visit(Vertex u) {
        
        this->discover(u);

        // Visit adjacent vertices to u
        Graph::adjacency_iterator v_it, v_it_end;
        std::tie(v_it,v_it_end) = boost::adjacent_vertices(u, *(this->gr));

        for(; v_it != v_it_end; v_it++){
            if(this->color[*v_it] == DFS::WHITE){
                this->parent[*v_it] = u;
                this->visit(*v_it);
                // update low
                this->update_low(u, *v_it);
            }
            // (u,v) is back arc AND v is not parent of u 
            else if (this->is_back_arc(u, *v_it) && this->parent[u] != *v_it){
                this->update_low(u, *v_it);
            }
        }

        this->finish(u);
    }
    
    /* ====================================================
     *                   Helper Methods
     * ==================================================== */

    // Update low if smaller candidate
    // 
    // low[u] := min {
    //           d[u]
    //           d[w] , where {v, w} is a back edge and v is a descendant of u
    // }
    void update_low(Vertex u, Vertex v) {
        if(low[u] > low[v]) low[u] = low[v];
    }

    bool is_back_arc(Vertex u, Vertex v) {
        return (discovery_time[v] < discovery_time[u]) \
             & (finish_time[u] < finish_time[v]);
    }
};

void find_cut_vertexes(Graph &g) {
  for (const auto& vertex : boost::make_iterator_range(boost::vertices(g))) {
    g[vertex].cutvertex = false;
  }

}

void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges) {
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
