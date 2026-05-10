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

class MinSlideCost : public HeuristikCost{
public:
    float cost(const Board& board, const Graph& graph, const State& s) override;
private:
    vector<int> minCostRow;   // min cost tile valid untuk setiap baris
    vector<int> minCostCol;   // min cost tile valid untuk setiap kolom
    bool precomputed = false;
    void precompute(const Board& board);
};
