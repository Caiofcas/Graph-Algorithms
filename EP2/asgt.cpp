#include "asgt.h"
#include <iostream>
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
    std::vector<int> num_children;
    std::vector<int> low;
    std::vector<bool> cut_vertex;
    std::vector<Vertex> parent;

    DFS(Graph* gr) {
        this->n = boost::num_vertices(*gr);
        this->gr = gr;
        this->set_initial_state();
    };


    // Helper to reset DFS if needed
    void set_initial_state() {
        this->next = this->time = 0;
        this->color = std::vector<int>(this->n, WHITE);
        this->discovery_time = std::vector<int>(this->n, 0);
        this->finish_time = std::vector<int>(this->n, 0);
        this->num_children = std::vector<int>(this->n, 0);
        this->low = std::vector<int>(this->n, 0);
        this->cut_vertex = std::vector<bool>(this->n, false);
        this->parent = std::vector<Vertex>(this->n, this->n);
    }

    /* ====================================================
     *                      Label Edges
     * ==================================================== */

    void find_cycle(Edge back_edge, int* cur_count) {
        std::stack<Edge> stack;
        int label = -1;
        Vertex cur_vert = back_edge.m_target;
        Edge edge_it;

        stack.push(back_edge);
        // Go up tree edges from target until we reach source, 
        // the set traveled + back_edge is a cycle
        while (parent[cur_vert] != back_edge.m_source) {
            edge_it = boost::edge(cur_vert, parent[cur_vert], (*gr)).first;
            std::cout << "source: " << edge_it.m_source;
            std::cout << " target: " << edge_it.m_target;
            std::cout << " cur:" << cur_vert << std::endl;
            cur_vert = edge_it.m_target;
        }
        //  do {
        //     v = (*stack).top();
        //     (*stack).pop();
        //     (*in_stack)[v] = false;
        //     (*sc_labeling)[v] = *nscc;
        // } while (v != u);
    }


    void label_edges_by_bcc(){
        int bcc_count = 0;
        
        for (const auto& edge : boost::make_iterator_range(boost::edges(*gr))) {
            if (!(*gr)[edge].bcc) { // check if it was already set
                if (is_bridge(edge)){
                    (*gr)[edge].bcc = ++bcc_count;
                } else if (is_back_edge(edge.m_source, edge.m_target)) {
                    find_cycle(edge, &bcc_count);
                }
            } 
        }


    }

    /* ====================================================
     *                 Performing DFS Methods
     * ==================================================== */

    // Visit u
    void discover(Vertex u) {
        this->time++;
        this->discovery_time[u] = this->low[u] = this->time;
        this->color[u] = DFS::GRAY;
        // std::cout << "[discover] Vertex " << u+1 << " d: " << time << std::endl;
    }

 
    // Finish visiting u
    void finish(Vertex u) {
        this->time++;
        this->finish_time[u] = time;
        this->color[u] = DFS::BLACK;
        // std::cout << "[finish] Vertex " << u+1 << " time: " << time << std::endl;
    }

 
    // DFS visit method
    void visit(Vertex u) {
        this->discover(u);

        // Visit adjacent vertices to u
        Graph::adjacency_iterator v_it, v_it_end;
        std::tie(v_it,v_it_end) = boost::adjacent_vertices(u, *(this->gr));

        for(; v_it != v_it_end; v_it++){
            // std::cout << "[visit] On vertex " << u+1 << " visiting " << *v_it+1 << std::endl;

            // Visit undiscovered vertexes
            if (this->color[*v_it] == DFS::WHITE){
                this->num_children[u]++;
                this->parent[*v_it] = u;
                this->visit(*v_it);

                if (low[*v_it] < discovery_time[u]) {
                    // child can reach vertex up the tree

                    // std::cout << "[visit] Vertex " << *v_it+1 << " can reach up the tree";
                    // std::cout << " up to Vertex discovered in time " << low[*v_it] << ". ";
                    // std::cout << "So Vertex " << u+1 << " can also reach up the tree to that vertex." << std::endl;
                    
                    // only update lowlink if smaller
                    if (low[u] > low[*v_it]) low[u] = low[*v_it];
                } else {
                    // child can't reach vertex up the tree
                    // so it is a cutvertex by this criteria, 
                    // must still check if it's root later.

                    // std::cout << "[visit] Vertex " << u+1 << " is a cut vertex due to " << *v_it+1 << std::endl;
                    cut_vertex[u] = true;
                }
            }

            // Is back edge and not to parent. Found a way up the tree
            if (parent[u] != *v_it && is_back_edge(u, *v_it)){
                // std::cout << "[visit] Back edge from " << u+1 << " to " << *v_it+1 << std::endl;
                if (discovery_time[*v_it] < low[u])
                    low[u] = discovery_time[*v_it];
                
                // std::cout << "[visit] low[" << u+1 << "]: " << low[u] << std::endl;
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
     *                   Statement Methods
     * ==================================================== */

    bool is_back_edge(Vertex cur_vert, Vertex other_vert) {
        // std::cout << "[is_back_edge] " << cur_vert+1 << " " << other_vert+1 << std::endl;
        // std::cout <<  "[is_back_edge] " << (discovery_time[cur_vert] > discovery_time[other_vert]) << std::endl;

        return discovery_time[cur_vert] > discovery_time[other_vert];
    }


    bool is_dfs_root(Vertex u) {
        return this->parent[u] == (unsigned int) this->n;
    }


    bool is_bridge(Edge e) {
        if (is_back_edge(e.m_source, e.m_target)) return false;
        
        // Is tree edge (m_source discovered sooner)
        return low[e.m_target] > discovery_time[e.m_source];
    }


    bool is_cutvertex(Vertex u) {
        // std::cout << "[is_cutvertex] Vertex " << u+1 << std::endl;
        if (is_dfs_root(u)) return num_children[u] >= 2;
        // std::cout << "[is_cutvertex] is not dfs root." << std::endl;
        return cut_vertex[u];
    }

};


void find_cut_vertexes(Graph &g, DFS *dfs) {
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(g))) {
        g[vertex].cutvertex = dfs->is_cutvertex(vertex);
    }
}


void find_bridges(Graph &g, DFS *dfs) {
    for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
        g[edge].bridge = dfs->is_bridge(edge);
    }
}


void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges) {

    DFS dfs(&g);
    dfs.perform_dfs();

    if (fill_cutvxs) {
        find_cut_vertexes(g, &dfs);
        return;
    }

    if (fill_bridges) {
        find_bridges(g, &dfs);
        return;
    }
    
    dfs.label_edges_by_bcc();
}
