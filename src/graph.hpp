#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
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
    vector<int> checkpoints;
    int cost;
};

struct Graph {
    unordered_set<Node, NodeHash> nodes;
    unordered_map<Node, vector<Edge>, NodeHash> adj;
    Node start;
    Node goal;
    int num_checkpoints;        // total jumlah checkpoint di map
};