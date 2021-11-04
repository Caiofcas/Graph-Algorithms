#include "asgt.h"
#include <iostream>

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
    std::vector<int> num_children;
    std::vector<int> low;
    std::vector<Vertex> parent;

    DFS(Graph* gr) {
        this->n = boost::num_vertices(*gr);
        this->gr = gr;
        this->set_initial_state();
    };

    void set_initial_state() {
        this->next = this->time = 0;
        this->color = std::vector<int>(this->n, WHITE);
        this->discovery_time = std::vector<int>(this->n, 0);
        this->finish_time = std::vector<int>(this->n, 0);
        this->num_children = std::vector<int>(this->n, 0);
        this->low = std::vector<int>(this->n, 0);
        this->parent = std::vector<Vertex>(this->n, this->n);
    }


    /* ====================================================
     *                 Performing DFS Methods
     * ==================================================== */

    // Visit u
    void discover(Vertex u) {
        this->time++;
        this->discovery_time[u] = this->time;
        this->low[u] = this->time;
        this->color[u] = DFS::GRAY;
        std::cout << "[discover] Vertex " << u+1 << " time: " << time << std::endl;
    }

    // Finish visiting u
    void finish(Vertex u) {
        this->time++;
        this->finish_time[u] = time;
        this->color[u] = DFS::BLACK;
        std::cout << "[finish] Vertex " << u+1 << " time: " << time << std::endl;
    }

    // DFS visit method
    void visit(Vertex u) {
        this->discover(u);

        // Visit adjacent vertices to u
        Graph::adjacency_iterator v_it, v_it_end;
        std::tie(v_it,v_it_end) = boost::adjacent_vertices(u, *(this->gr));

        for(; v_it != v_it_end; v_it++){

            std::cout << "[visit] On vertex " << u+1 << " visiting " << *v_it+1 << std::endl;
            if(this->color[*v_it] == DFS::WHITE){
                this->num_children[u]++;
                this->parent[*v_it] = u;
                this->visit(*v_it);
                // update low
                this->update_low(u, *v_it);
            }
            // (u,v) is back arc AND v is not parent of u 
            else if (this->parent[u] != *v_it && this->is_back_arc(u, *v_it)){
                std::cout << "[visit] update low[" << u+1 << "] based on " << *v_it+1 << std::endl;
                std::cout << "[visit] low[" << u+1 << "]: " << low[u] << std::endl;
                if (low[u] >= discovery_time[*v_it]){
                    low[u] = discovery_time[*v_it];
                }
                std::cout << "[visit] new low[" << u+1 << "]: " << low[u] << std::endl;
            }
        }

        this->finish(u);
    }


    // Main DFS method
    void perform_dfs() {
        if (this->time) this->set_initial_state();
        
        Graph::vertex_iterator v_it, v_it_end;

        std::tie(v_it, v_it_end) = boost::vertices(*(this->gr));
        for(; v_it != v_it_end; v_it++){
            if(this->color[*v_it] == DFS::WHITE){
                this->visit(*v_it);
            }
        }
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
        std::cout << "[update_low] u: "<< u+1 << " v: " << v+1 << std::endl;
        std::cout << "[update_low] low[" << u+1 << "]: " << low[u] << std::endl;
        std::cout << "[update_low] low[" << v+1 << "]: " << low[v] << std::endl;
        if(low[u] > low[v]) low[u] = low[v];
        std::cout << "[update_low] new low[" << u+1 << "]: " << low[u] << std::endl;        
    }

    bool is_back_arc(Vertex u, Vertex v) {
        std::cout << "[is_back_arc] " << u+1 << " " << v+1 << std::endl;
        std::cout << "[is_back_arc] d[" << u+1 << "]: " << discovery_time[u];
        std::cout << " f[" << u+1 << "]: "  << finish_time[u] << std::endl;
        std::cout << "[is_back_arc] d[" << v+1 << "]: " << discovery_time[v];
        std::cout << " f[" << v+1 << "]: "  << finish_time[v] << std::endl;
        std::cout <<  "[is_back_arc] " << ((discovery_time[v] < discovery_time[u]) && (finish_time[u] <= finish_time[v])) << std::endl;

        return (discovery_time[v] < discovery_time[u]) \
             && (finish_time[u] <= finish_time[v]);
    }

    bool is_dfs_root(Vertex u) {
        return this->parent[u] == (unsigned int) this->n;
    }

    bool is_cutvertex(Vertex u) {
        /* 2 cases for being cutvertex:
         * 
         * 1. Is root and has 2+ children;
         * 2. Is not root and low[u] < d[u]
         * 
         */

        if (this->is_dfs_root(u)){
            std::cout << "[is_cutvertex] Vertex " << u+1 << " is dfs root" << std::endl; 
            return num_children[u] >= 2;
        }
        std::cout << "[is_cutvertex] Vertex " << u+1 << " is not dfs root" << std::endl; 
        
        std::cout << "[is_cutvertex] low[" << u+1 << "]: " << low[u] << std::endl;
        std::cout << "[is_cutvertex] d[" << u+1 << "]: " << discovery_time[u] << std::endl;
       std::cout << "[is_cutvertex] Vertex " << u+1 << " : " << (low[u] >= discovery_time[u]) << std::endl;

        return low[u] >= discovery_time[u];
    }
};

void find_cut_vertexes(Graph &g, DFS *dfs) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(g))) {
        g[vertex].cutvertex = dfs->is_cutvertex(vertex);
    }
}

void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges) {

    DFS dfs(&g);
    dfs.perform_dfs();

    /* fill everything with dummy values */
    if (fill_cutvxs)
    {
        find_cut_vertexes(g, &dfs);
    };
    
    for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
        g[edge].bcc = 0;
        g[edge].bridge = false;
    }
}
