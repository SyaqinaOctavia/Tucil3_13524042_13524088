#include "include/searchAlgorithm.hpp"
#include "include/heuristikCost.hpp"
#include <queue>
#include <iostream>
#include <algorithm>

using namespace std;

SearchResult SearchAlgorithm::searchUCS(const Board& board, const Graph& g){
    unordered_map<State, State, StateHash> parentState; //untuk reconstruct path
    unordered_map<State, Edge, StateHash> parentEdge;   //untuk reconstruct path
    // Catat informasi search
    SearchResult res;
    res.found = false; 
    res.totalCost = 0;
    res.iterations = 0;
    res.startPos = g.start;

    // Catat visited Node beserta costnya
    unordered_map<State, float, StateHash> visitedNode;
    
    // Min priority queue
    std::priority_queue<NodeStateQueue, vector<NodeStateQueue>, greater<NodeStateQueue>> priorQ;
    
    // Push start node ke queue
    State startState;
    startState.pos = g.start;
    startState.nextCheckpoint = 0;
    float gn = 0.0; 
    float fn = gn;

    priorQ.push({startState, gn, 0, fn});  //hn = 0
    
    visitedNode[startState] = 0;

    // Proses sampai kosong atau sampai ketemu goal
    while (!(priorQ.empty())){
        NodeStateQueue currNQ = priorQ.top();  //expand node yang punya cost minimum
        priorQ.pop();
        State currState = currNQ.state;
        float currCost = currNQ.gCost;   //cumulative cost sampai currState

        if (visitedNode.count(currState) && currCost > visitedNode[currState]) continue; //udah pernah dilewatin dengan cost lebih baik

        res.iterations++;

        // Kalau udah ketemu goal, berhenti
        if (currState.pos == g.goal && allCheckpointVisited(currState, g)){
            res.found = true;
            res.totalCost = currCost;
            res.path = reconstructPath(parentState, parentEdge, startState, currState);

            return res;
        }
        // Daftarkan node tetangga ke queue
        for (const Edge& e : g.adj.at(currState.pos)){ 
            State nState = nextState(board, currState, e);
            gn = currCost + e.cost;  //parent cost + edge cost
            fn = gn;
            if (!visitedNode.count(nState) || fn < visitedNode[nState]){  //blm pernah dilewatin atau costnya lebih baik
                visitedNode[nState] = fn;
                parentState[nState] = currState;
                parentEdge[nState] = e;
              
                priorQ.push({nState, gn, 0, fn}); 
            }
        }
    }
    // priorQ.empty(), not found
    return res;
}

vector<Edge> reconstructPath(const unordered_map<State, State, StateHash>& parentState, const unordered_map<State, Edge, StateHash>& parentEdge, const State& start, const State& goal){
    vector<Edge> path;
    State curr = goal;
    while (!(curr == start)) {
        path.push_back(parentEdge.at(curr));
        curr = parentState.at(curr);
    }
    reverse(path.begin(), path.end());

    return path;
}

bool allCheckpointVisited(const State& currState, const Graph& g){
    return (currState.nextCheckpoint >= g.num_checkpoints);
}


