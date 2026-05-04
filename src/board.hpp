#pragma once

#include <string>
#include "graph.hpp"

using namespace std;

struct Board {
    vector<string> grid;          // grid[r] adalah string baris ke-r
    vector<vector<int>> cost;     // cost[r][c]
    int rows;
    int cols;
    Node start;                             // posisi Z
    Node goal;                              // posisi O
    vector<Node> checkpoints;          // checkpoints[i] = posisi angka i
};