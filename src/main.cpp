#include <iostream>
#include <string>
#include <chrono>
#include <memory>
#include "include/gui.hpp"
#include "include/loadSave.hpp"
#include "include/board.hpp"
#include "include/graph.hpp"
#include "include/searchAlgorithm.hpp"
#include "include/heuristikCost.hpp"
#include "include/state.hpp"

using namespace std;

// ---- Window dimensions ----
static const int SCR_W = 1200;
static const int SCR_H =  700;

// ---- Panel layout ----
static const Rectangle PANEL_LEFT  = {   0, 0, 215, SCR_H };
static const Rectangle PANEL_BOARD = { 215, 0, 690, SCR_H };
static const Rectangle PANEL_RIGHT = { 905, 0, 315, SCR_H };

static bool isInsideBoard(const Board& board, const Node& node) {
    return node.row >= 0 && node.col >= 0 &&
           node.row < board.rows && node.col < board.cols;
}

static bool validateBoard(const Board& board, string& errorMsg) {
    if (board.rows <= 0 || board.cols <= 0) {
        errorMsg = "Dimensi board tidak valid.";
        return false;
    }
    if ((int)board.grid.size() != board.rows ||
        (int)board.cost.size() != board.rows) {
        errorMsg = "Jumlah baris grid/cost tidak sesuai.";
        return false;
    }
    for (int r = 0; r < board.rows; ++r) {
        if ((int)board.grid[r].size() != board.cols) {
            errorMsg = "Panjang baris grid tidak sesuai.";
            return false;
        }
        if ((int)board.cost[r].size() != board.cols) {
            errorMsg = "Panjang baris cost tidak sesuai.";
            return false;
        }
        for (int c = 0; c < board.cols; ++c) {
            char ch = board.grid[r][c];
            bool validChar = ch == '*' || ch == '.' || ch == 'X' || ch == 'L' ||
                             ch == 'O' || ch == 'Z' ||
                             (ch >= '0' && ch <= '9');
            if (!validChar) {
                errorMsg = "Karakter board tidak valid.";
                return false;
            }
        }
    }
    if (!isInsideBoard(board, board.start)) {
        errorMsg = "Start Z tidak ditemukan.";
        return false;
    }
    if (!isInsideBoard(board, board.goal)) {
        errorMsg = "Goal O tidak ditemukan.";
        return false;
    }

    int checkpointCount = 0;
    for (const Node& checkpoint : board.checkpoints) {
        if (checkpoint.row != -1) ++checkpointCount;
    }
    for (int i = 0; i < checkpointCount; ++i) {
        if (!isInsideBoard(board, board.checkpoints[i])) {
            errorMsg = "Checkpoint harus berurutan dari 0.";
            return false;
        }
    }
    return true;
}

// ============================================================
//  Execute the search pipeline (called from main loop when
//  gui.runRequested becomes true).
// ============================================================
static void runSearch(GuiState& gui) {
    // Reset everything
    gui.board       = Board();
    gui.graph       = Graph();
    gui.boardLoaded = false;
    gui.result      = SearchResult();
    gui.stepStates.clear();
    gui.anim        = AnimState();
    gui.appState    = AppState::IDLE;
    gui.errorMsg    = "";

    // ---- Load file ----
    loadFile(string(gui.fileInput), &gui.board);

    if (gui.board.grid.empty()) {
        gui.appState = AppState::INVALID_BOARD;
        gui.errorMsg = "File tidak bisa dibuka/empty: " + string(gui.fileInput);
        return;
    }

    string validationError;
    if (!validateBoard(gui.board, validationError)) {
        gui.appState = AppState::INVALID_BOARD;
        gui.errorMsg = validationError;
        return;
    }

    gui.boardLoaded = true;

    // ---- Build graph + run search (timed together) ----
    auto t0 = chrono::high_resolution_clock::now();

    buildGraph(gui.board, gui.graph);

    // Build heuristic
    unique_ptr<HeuristikCost> heuristic;
    DistanceTable distTable;

    const bool needHeur = (gui.algoSelected != 0);
    if (needHeur) {
        if (gui.heurSelected == 0) {
            heuristic.reset(new ManhattanCost());
        } else if (gui.heurSelected == 1) {
            heuristic.reset(new EuclideanCost());
        } else {
            // H3: pre-compute distance table
            distTable = getDistanceTable(gui.graph, gui.board);
            heuristic.reset(new DijkstraCost(&distTable));
        }
    }

    SearchAlgorithm solver(heuristic.get());

    if      (gui.algoSelected == 0) gui.result = solver.searchUCS  (gui.board, gui.graph);
    else if (gui.algoSelected == 1) gui.result = solver.searchGBFS (gui.board, gui.graph);
    else                            gui.result = solver.searchAStar (gui.board, gui.graph);

    auto t1 = chrono::high_resolution_clock::now();
    gui.executionTimeMs =
        chrono::duration<double, milli>(t1 - t0).count();
    gui.result.executionTimeMs = (long long)gui.executionTimeMs;

    // ---- Update app state ----
    if (gui.result.found) {
        gui.appState = AppState::RESULT_FOUND;

        // Pre-compute State at every step so animation is instant
        State s;
        s.pos            = gui.graph.start;
        s.nextCheckpoint = 0;
        gui.stepStates.push_back(s);

        for (const Edge& e : gui.result.path) {
            auto maybe = nextState(gui.board, s, e);
            if (maybe) s = *maybe;
            gui.stepStates.push_back(s);
        }
    } else {
        gui.appState = AppState::RESULT_NOT_FOUND;
    }
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    InitWindow(SCR_W, SCR_H, "Slider Puzzle Solver");
    initGuiFont();
    SetTargetFPS(60);

    GuiState gui;

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        // ---- Handle run request (blocks one frame intentionally) ----
        if (gui.runRequested) {
            gui.runRequested = false;
            runSearch(gui);
        }

        // ---- Handle save request ----
        if (gui.saveRequested) {
            gui.saveRequested = false;
            saveResult(gui);
        }

        // ---- Draw ----
        BeginDrawing();
        ClearBackground({10, 13, 22, 255});

        drawLeftPanel (gui, PANEL_LEFT);
        drawBoardPanel(gui, PANEL_BOARD, dt);
        drawRightPanel(gui, PANEL_RIGHT);

        EndDrawing();
    }

    unloadGuiFont();
    CloseWindow();
    return 0;
}
