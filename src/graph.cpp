#include "include/graph.hpp"
#include "include/board.hpp"

using namespace std;

static inline pair<int,int> getComponentDir(char dir) {
    switch(dir) {
        case 'U': return {-1, 0};
        case 'D': return { 1, 0};
        case 'L': return { 0,-1};
        case 'R': return { 0, 1};
    }
    return {0,0};
}

vector<Edge> getSlideEdges(const Board& board, Node n, char dir) {
    vector<Edge> edges;
    vector<pair<int,int>> pathTiles;
    int totalCost = 0;

    int r = n.row;
    int c = n.col;
    pathTiles.push_back({r, c});
    // Slide terus sampai ada hambatan
    while (true) {
        pair<int,int> comp = getComponentDir(dir);
        int dr = comp.first;
        int dc = comp.second;
        int try_row = r + dr;
        int try_col = c + dc;
        // Handle keluar map
        if (try_row<0 || try_col<0 || try_row>=board.rows || try_col>=board.cols){
            return edges;
        }
        // Handle ketemu batu
        if (board.grid[try_row][try_col] == 'X'){
            break; 
        } 
        // Handle ketemu lava, tp ttp mencatat ke pathTiles dan hitung cost di Lava tile
        if (board.grid[try_row][try_col] == 'L'){
            return edges;
        }

        r = try_row;
        c = try_col;
        pathTiles.push_back({r, c});
        totalCost += board.cost[r][c];
    }
    if (pathTiles.size() > 1) {
        Edge e;
        e.curr = n;
        e.next = Node{r, c};
        e.direction = dir;
        e.tiles = pathTiles;
        e.cost = totalCost;
        edges.push_back(e);
    }
    
    return edges;
}

// Build graph dari board 
void buildGraph(const Board& board, Graph& graph) {
    graph.start = board.start;
    graph.goal = board.goal;
    graph.num_checkpoints = 0;
    
    // Daftarkan Node yaitu seluruh tile di mana pemain bisa berdiri
    for (int r=0; r<board.rows; r++) {
        for (int c=0; c<board.cols; c++) {
            if (board.grid[r][c] != 'X' && board.grid[r][c] != 'L') {
                graph.nodes.insert(Node{r, c});
            }
        }
    }
    
    for (const Node& n : board.checkpoints) {
        if (n.row != -1) graph.num_checkpoints++;
    }
    
    // Build adjacency list dari edge yang valid
    char possible_dir[4] = {'U', 'D', 'L', 'R'};
   
    for (const Node& n : graph.nodes) graph.adj.emplace(n, vector<Edge>());

    for (const Node& n : graph.nodes) {
        for (int i=0; i<4; i++) {
            vector<Edge> slide_edges = getSlideEdges(board, n, possible_dir[i]);
            if (!slide_edges.empty()) {
                vector<Edge> &vec = graph.adj[n];
                for (const Edge& e : slide_edges) {
                    vec.push_back(e);  
                }
            }
        }
    }
}