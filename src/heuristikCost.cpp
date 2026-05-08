#include "include/heuristikCost.hpp"
#include <cmath>
#include <limits>
#include <algorithm>
#include <queue>

using namespace std;

static inline float manhattan(const Node& a, const Node& b){
    return abs(a.row - b.row) + abs(a.col - b.col);
}

static inline float euclidean(const Node& a, const Node& b){
    int dr = a.row - b.row;
    int dc = a.col - b.col;
    return sqrt((float)(dr*dr + dc*dc));
}

float ManhattanCost::cost(const Board& board, const Graph& graph, const State& s){
    float h = 0.0f;
    Node curr = s.pos;
    // Cost dari checkpoint saat ini: checkpoint[i] -> checkpoint[i+1] ->....-> goal
    for (int i=s.nextCheckpoint; i<graph.num_checkpoints; i++){
        Node cp = board.checkpoints[i];
        if (cp.row == -1) continue; 
        h += manhattan(curr, cp);
        curr = cp;
    }
    h += manhattan(curr, graph.goal);

    return h;
}

float EuclideanCost::cost(const Board& board, const Graph& graph, const State& s){
    float h = 0.0f;
    Node curr = s.pos;
    // Cost dari checkpoint saat ini: checkpoint[i] -> checkpoint[i+1] ->....-> goal
    for (int i=s.nextCheckpoint; i<graph.num_checkpoints; i++){
        Node cp = board.checkpoints[i];
        if (cp.row == -1) continue;
        h += euclidean(curr, cp);
        curr = cp;
    }
    h += euclidean(curr, graph.goal);

    return h;
}

float DijkstraCost::cost(const Board& board, const Graph& graph, const State& s){
    float h = 0.0f;
    Node curr = s.pos;
    // Cost dari checkpoint saat ini: checkpoint[i] -> checkpoint[i+1] ->....-> goal
    for (int i = s.nextCheckpoint; i < graph.num_checkpoints; i++){
        Node cp = board.checkpoints[i];
        if (cp.row == -1) continue;
        const unordered_map<Node, int, NodeHash>& cpDist = distTable->distToCheckpoint.at(i);
        auto it = cpDist.find(curr);
        if (it == cpDist.end() || it->second == numeric_limits<int>::max()){
            return numeric_limits<float>::infinity();  // tidak reachable
        }
        h += it->second;
        curr = cp;
    }
    auto it = distTable->distToGoal.find(curr);
    if (it == distTable->distToGoal.end() || it->second == numeric_limits<int>::max()){
        return numeric_limits<float>::infinity();
    }
    h += it->second;

    return h;
}
DistanceTable getDistanceTable(const Graph& g, const Board& board){
    DistanceTable distTable;

    // Compute dist goal ke tiap node
    for (const Node& n : g.nodes){
        distTable.distToGoal[n] = numeric_limits<int>::max(); //inisiasi dengan infinity
    }
    // Jarak dari node goal ke dirinya sendiri = 0
    distTable.distToGoal[g.goal] = 0;

    // Min priority queue
    std::priority_queue<NodeQueue, vector<NodeQueue>, greater<NodeQueue>> goalQ;
    goalQ.push({g.goal, 0}); 
    while (!goalQ.empty()){
        NodeQueue curr = goalQ.top();  //node yang punya dist terkecil diproses
        int currDist = curr.dist;
        Node u = curr.node;
        goalQ.pop();

        if (currDist > distTable.distToGoal[u]) continue;

        // Update jarak ke node tetangga
        for (const Edge& e : g.rev_adj.at(u)){ 
            Node v = e.next;
            int newDist = distTable.distToGoal[u] + e.cost;
            if (newDist < distTable.distToGoal[v]){
                distTable.distToGoal[v] = newDist;
                goalQ.push({v, newDist});
            }
        }
    }

    // Compute dist masing-masing checkpoint ke tiap node
    for (const Node& cp : board.checkpoints){
        if (cp.row == -1) continue;

        unordered_map<Node, int, NodeHash> distCheckpoint_i; 

        for (const Node& n : g.nodes){
            distCheckpoint_i[n] = numeric_limits<int>::max();  //inisiasi dengan infinity
        }
        // Jarak dari node checkpoint ke dirinya sendiri = 0
        distCheckpoint_i[cp] = 0;

        priority_queue<NodeQueue, vector<NodeQueue>, greater<NodeQueue>> cpQ;
        cpQ.push({cp, 0});
        while (!cpQ.empty()){
            NodeQueue curr = cpQ.top();
            cpQ.pop();
            int currDist = curr.dist;
            Node u = curr.node;
            
            if (currDist > distCheckpoint_i[u]) continue;

            // Update jarak ke node tetangga
            for (const Edge& e : g.rev_adj.at(u)){
                Node v = e.next;
                int newDist = distCheckpoint_i[u] + e.cost;
                if (newDist < distCheckpoint_i[v]){
                    distCheckpoint_i[v] = newDist;
                    cpQ.push({v, newDist});
                }
            }
        }

        distTable.distToCheckpoint.push_back(distCheckpoint_i);
    }

    return distTable;
}

