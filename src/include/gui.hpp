#pragma once

#include <raylib.h>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include "board.hpp"
#include "graph.hpp"
#include "searchAlgorithm.hpp"
#include "state.hpp"

using namespace std;

#define MAX_INPUT_CHARS 80

// Steps per second
inline constexpr float ANIM_SPEEDS[] = { 2.0f, 4.0f, 8.0f, 32.0f };
inline constexpr const char* SPEED_LABELS[] = { "2x", "4x", "8x", "32x" };
inline constexpr int NUM_SPEEDS = 4;

enum class AppState {
    IDLE,
    RESULT_FOUND,
    RESULT_NOT_FOUND,
    INVALID_BOARD
};

struct AnimState {
    int  currentStep = 0;   // n = after n-th edge
    bool playing     = false;
    int  speedIndex  = 1;   
    float timer      = 0.f;
};

struct GuiState {
    // ============ Input ============
    char fileInput[MAX_INPUT_CHARS + 1];
    int  fileInputLen    = 0;
    bool fileInputActive = false;

    int algoSelected = 0;   // 0=UCS  1=GBFS  2=A*
    int heurSelected = 0;   // 0=H1   1=H2    2=H3

    // ============ App state ============
    AppState appState = AppState::IDLE;
    string   errorMsg;


    Board board;
    Graph graph;
    bool  boardLoaded = false;

    SearchResult result;
    double       executionTimeMs = 0.0;

    vector<State> stepStates;
    AnimState anim;

    // ============ For main loop ============
    bool runRequested  = false;
    bool saveRequested = false;

    GuiState() { memset(fileInput, 0, sizeof(fileInput)); }
};

void initGuiFont();
void unloadGuiFont();
void drawLeftPanel (GuiState& gui, Rectangle panel);
void drawBoardPanel(GuiState& gui, Rectangle panel, float dt);
void drawRightPanel(GuiState& gui, Rectangle panel);
void saveResult (const GuiState& gui);
static void DrawMoveSequencePanel(int x, int y, int w, int h, GuiState& gui);
