#pragma once

#include "graph.hpp"
#include "state.hpp"
#include "board.hpp"

using namespace std;

class HeuristikCost {
public:
    virtual float cost(const Board& board, const Graph& graph, const State& s) = 0;
    virtual ~HeuristikCost() {};
};

class ManhattanCost : public HeuristikCost{
public:
    float cost(const Board& board, const Graph& graph, const State& s) override;
};

class EuclideanCost : public HeuristikCost{
public:
    float cost(const Board& board, const Graph& graph, const State& s) override;
};

struct DistanceTable{
    unordered_map<Node, int, NodeHash> distToGoal; //catat dist tiap Node ke Goal
    vector<unordered_map<Node,int,NodeHash>> distToCheckpoint;  //catat dist tiap Node ke tiap Checkpoint
};

class DjikstraCost : public HeuristikCost{
private:
    DistanceTable* distTable;  //precompute dist dari tiap Node ke tiap checkpoint dan ke goal
public:
    DjikstraCost(DistanceTable* distTable){
        distTable = distTable;
    }
    float cost(const Board& board, const Graph& graph, const State& s) override;
};


