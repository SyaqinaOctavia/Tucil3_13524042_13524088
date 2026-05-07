#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Board;

using namespace std;

struct Node{
    int row, col;
    
    bool operator==(const Node& other) const {
        return row == other.row && col == other.col;
    }
};

struct NodeHash {
    std::size_t operator()(const Node& n) const {
        return n.row * 1000 + n.col;
    }
};

struct Edge{
    Node curr, next;
    char direction;
    vector<pair<int,int>> tiles;
    // vector<int> checkpoints;
    int cost;
};

struct Graph {
    unordered_set<Node, NodeHash> nodes;
    unordered_map<Node, vector<Edge>, NodeHash> adj;
    unordered_map<Node, vector<Edge>, NodeHash> rev_adj;  //untuk dijkstra heuristik
    Node start;
    Node goal;
    int num_checkpoints;        // total jumlah checkpoint di map
};

bool tilesContainNode(const vector<pair<int,int>>& tiles, const Node& n);

optional<Edge> getSlideEdge(const Board& board, Node n, char dir);

void buildGraph(const Board& board, Graph& graph);