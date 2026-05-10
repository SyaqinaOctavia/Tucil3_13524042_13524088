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

// Hitung state berikutnya setelah edge dilewati. Return nullopt kalau slide ini melanggar urutan checkpoint (mis. melewati '1' sebelum '0' → game over).
// edge.tiles[0] adalah tile awal (tidak dihitung sebagai "dilewati"), tiles[1..n] adalah tile yang benar-benar dilewati selama sliding.
inline optional<State> nextState(const Board& board, const State& curr, const Edge& edge){
    State next;
    next.pos = edge.next;
    next.nextCheckpoint = curr.nextCheckpoint;

    for (size_t i = 1; i < edge.tiles.size(); ++i){
        int r = edge.tiles[i].first;
        int c = edge.tiles[i].second;
        char ch = board.grid[r][c];
        if (ch >= '0' && ch <= '9'){
            int num = ch - '0';
            if (num == next.nextCheckpoint){
                next.nextCheckpoint++;          //checkpoint diinjak sesuai urutan
            } else if (num > next.nextCheckpoint){
                return nullopt;                  //melewati checkpoint > nextCheckpoint = game over
            }
            //kalau num < nextCheckpoint: tile udah pernah dilewati, sekarang dianggap tile biasa
        }
    }

    return next;
}