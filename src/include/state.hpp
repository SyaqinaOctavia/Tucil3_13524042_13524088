#pragma once

#include "graph.hpp"
#include "board.hpp"

using namespace std;

struct State{
    Node pos;
    int nextCheckpoint;

    bool operator==(const State& other) const{
        return pos == other.pos && nextCheckpoint == other.nextCheckpoint;
    }
};
struct StateHash{
    size_t operator()(const State& s) const{
        return s.pos.row * 100000 + s.pos.col * 100 + s.nextCheckpoint;
    }
};

inline State nextState(const Board& board, const State& curr, const Edge& edge){
    State next;
    next.pos = edge.next;
    next.nextCheckpoint = curr.nextCheckpoint;

    if (curr.nextCheckpoint < board.checkpoints.size()) {
        Node cp = board.checkpoints[curr.nextCheckpoint];

        if (tilesContainNode(edge.tiles, cp)) {
            next.nextCheckpoint++;
        }
    }

    return next;
}