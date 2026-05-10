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

// Menghitung min cost tile valid untuk setiap baris dan kolom
void MinSlideCost::precompute(const Board& board){
    minCostRow.assign(board.rows, numeric_limits<int>::max());
    minCostCol.assign(board.cols, numeric_limits<int>::max());

    for (int r = 0; r < board.rows; r++){
        for (int c = 0; c < board.cols; c++){
            char cell = board.grid[r][c];
            // Hanya tile yang bisa dilewati (bukan X dan bukan L)
            if (cell == 'X' || cell == 'L') continue;
            int tileCost = board.cost[r][c];
            minCostRow[r] = min(minCostRow[r], tileCost);
            minCostCol[c] = min(minCostCol[c], tileCost);
        }
    }
    precomputed = true;
}

// Estimasi cost minimum antar node, 
// menggunakan Manhattan distance dengan bobot cost tile minimum di baris/kolom yang relevan
static float minSlideCostSegment(const Node& A, const Node& B, const vector<int>& minCostRow, const vector<int>& minCostCol){
    int dr = abs(A.row - B.row);
    int dc = abs(A.col - B.col);

    // Ambil cost relevan
    int costRow = (minCostRow[A.row] == numeric_limits<int>::max()) ? 0 : minCostRow[A.row];
    int costCol = (minCostCol[A.col] == numeric_limits<int>::max()) ? 0 : minCostCol[A.col];

    return (float)(costRow * dc + costCol * dr);
}

float MinSlideCost::cost(const Board& board, const Graph& graph, const State& s){
    if (!precomputed) precompute(board);

    float h = 0.0f;
    Node curr = s.pos;
    // Cost dari checkpoint saat ini: checkpoint[i] -> checkpoint[i+1] ->....-> goal
    for (int i = s.nextCheckpoint; i < graph.num_checkpoints; i++){
        Node cp = board.checkpoints[i];
        if (cp.row == -1) continue;
        h += minSlideCostSegment(curr, cp, minCostRow, minCostCol);
        curr = cp;
    }
    h += minSlideCostSegment(curr, graph.goal, minCostRow, minCostCol);

    return h;
}

