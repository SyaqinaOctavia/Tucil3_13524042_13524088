#pragma once

#include "graph.hpp"
#include "state.hpp"
#include "heuristikCost.hpp"

using namespace std;

struct SearchResult{
    bool found = false;
    float totalCost = 0.0;
    int iterations = 0;
    Node startPos;
    vector<Edge> path;
    long long executionTimeMs = 0;
};
// Struct untuk min priority queue based on cost minimum, handle tie
struct NodeStateQueue{
    State state;
    float gCost;
    float hCost;
    float fCost;

    bool operator>(const NodeStateQueue& other) const{
        if (fCost != other.fCost) {return fCost > other.fCost;}
        //kalau tie
        return gCost < other.gCost;
    }
};

class SearchAlgorithm{
private:
    HeuristikCost* hCost;

public:
    SearchAlgorithm(HeuristikCost* h = nullptr) : hCost(h) {};

    SearchResult searchUCS(const Board& board, const Graph& graph);

    SearchResult searchGBFS(const Board& board, const Graph& graph);

    SearchResult searchAStar(const Board& board, const Graph& graph);
};

vector<Edge> reconstructPath(const unordered_map<State, State, StateHash>& parentState, const unordered_map<State, Edge, StateHash>& parentEdge, const State& start, const State& goal);
bool allCheckpointVisited(const State& currState, const Graph& g);
