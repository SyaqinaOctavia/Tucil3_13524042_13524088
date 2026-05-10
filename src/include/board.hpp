#pragma once

#include <string>
#include "graph.hpp"

using namespace std;

struct Board {
    vector<vector<char>> grid; 
    vector<vector<int>> cost;  
    int rows;
    int cols;
    Node start;   
    Node goal;  
    vector<Node> checkpoints; //checkpoints[i] = posisi angka i
    
    Board() : rows(0), cols(0), start{-1, -1}, goal{-1, -1}, checkpoints(10, Node{-1, -1}) {}
};