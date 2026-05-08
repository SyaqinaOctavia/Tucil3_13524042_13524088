#include <iostream>
#include "include/loadSave.hpp"
#include "include/board.hpp"
#include "include/graph.hpp"
#include "include/searchAlgorithm.hpp"

using namespace std;

int main(){
    Board board;
    cout << "Memuat data dari test/input/test1.txt...\n";
    loadFile("test1.txt", &board);

    Graph g;
    buildGraph(board, g);

    SearchAlgorithm solver;
    SearchResult result = solver.searchUCS(board, g);

    cout << "\nUCS Result:\n";
    cout << "  Found: " << (result.found ? "YES" : "NO") << "\n";
    cout << "  Total Cost: " << result.totalCost << "\n";
    cout << "  Iterations: " << result.iterations << "\n";
    cout << "  Path Length: " << result.path.size() << " edges\n";

    if (result.found) {
        cout << "\nPath (per step with direction):\n";
        Node current = g.start;
        for (size_t i = 0; i < result.path.size(); ++i) {
            const Edge& e = result.path[i];
            cout << "  Step " << (i+1) << ": (" << e.curr.row << "," << e.curr.col << ") "
                 << e.direction << " -> (" << e.next.row << "," << e.next.col << ") "
                 << "cost=" << e.cost << "\n";
        }
    }

    return 0;
}
