#include "include/gui.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

// ======================================================
//  FONT
// ======================================================
static Font gUiFont {};
static bool gUiFontLoaded = false;

static int UiMeasureText(const char* text, int fontSize) {
    if (!gUiFontLoaded) return MeasureText(text, fontSize);
    return (int)ceilf(MeasureTextEx(gUiFont, text, (float)fontSize, 1.0f).x);
}

static void UiDrawText(const char* text, int posX, int posY, int fontSize, Color color) {
    if (!gUiFontLoaded) {
        DrawText(text, posX, posY, fontSize, color);
        return;
    }
    DrawTextEx(gUiFont, text, {(float)posX, (float)posY}, (float)fontSize, 1.0f, color);
}

#define MeasureText UiMeasureText
#define DrawText UiDrawText

static bool isSupportedFontFile(const char* path) {
    int bytesRead = 0;
    unsigned char* data = LoadFileData(path, &bytesRead);
    if (data == nullptr || bytesRead < 4) {
        if (data != nullptr) UnloadFileData(data);
        return false;
    }

    bool supported =
        (data[0] == 0x00 && data[1] == 0x01 && data[2] == 0x00 && data[3] == 0x00) ||
        (data[0] == 't' && data[1] == 'r' && data[2] == 'u' && data[3] == 'e');

    UnloadFileData(data);
    return supported;
}

void initGuiFont() {
    const char* candidates[] = {
        "assets/fonts/ChickenPie.ttf",
        "../assets/fonts/ChickenPie.ttf",
        "../../assets/fonts/ChickenPie.ttf"
    };

    for (const char* path : candidates) {
        if (!FileExists(path)) continue;
        if (!isSupportedFontFile(path)) continue;
        gUiFont = LoadFontEx(path, 64, nullptr, 0);
        if (gUiFont.texture.id > 0) {
            SetTextureFilter(gUiFont.texture, TEXTURE_FILTER_BILINEAR);
            gUiFontLoaded = true;
            return;
        }
    }
}

void unloadGuiFont() {
    if (!gUiFontLoaded) return;
    UnloadFont(gUiFont);
    gUiFontLoaded = false;
}

//  =================== COLOR PALETTE ===================

static const Color C_BG          = { 251, 247, 242, 255 }; 
static const Color C_PANEL       = { 255, 255, 255, 255 };
static const Color C_PANEL_ALT   = { 245, 240, 234, 255 };

static const Color C_BORDER      = { 220, 214, 208, 255 };
static const Color C_SOFT_BROWN = { 181, 145, 105, 255 };

static const Color C_ACCENT      = { 168, 201, 170, 255 };
static const Color C_ACCENT_HOT  = { 126, 163, 129, 255 };

static const Color C_GREEN       = { 158, 196, 135, 255 };
static const Color C_GREEN_HOT   = { 118, 166, 100, 255 };

static const Color C_BLUE        = { 164, 174, 216, 255 };
static const Color C_BLUE_HOT    = { 128, 140, 190, 255 };

static const Color C_TEXT        = {  46,  42,  38, 255 };
static const Color C_TEXT_DIM    = { 72, 64, 58, 255 };

static const Color C_TEXT_ACC    = { 126, 163, 129, 255 };
static const Color C_TEXT_OK     = { 118, 166, 100, 255 };

static const Color C_TEXT_ERR    = { 210, 105, 105, 255 };
static const Color C_TEXT_WARN   = { 215, 170,  90, 255 };

//  ================= BOARD / TILE COLORS =================

static const Color C_TILE_EMPTY  = { 244, 239, 232, 255 };
static const Color C_TILE_ROCK   = { 130, 138, 118, 255 };
static const Color C_TILE_LAVA   = { 214, 118, 102, 255 };
static const Color C_TILE_GOAL   = { 234, 196, 102, 255 };
static const Color C_TILE_START  = { 148, 188, 138, 255 };
static const Color C_TILE_CP     = { 150, 165, 215, 255 };
static const Color C_TILE_TRAV   = { 196, 160, 215, 110 };
static const Color C_PLAYER      = { 201, 122, 122, 235 };
// ======================================================
//  LOW-LEVEL WIDGET HELPERS
// ======================================================

static bool Hovered(Rectangle r) {
    return CheckCollisionPointRec(GetMousePosition(), r);
}

// Clickable button. Returns true on mouse-release over it.
static bool Button(Rectangle r, const char* lbl, Color bg, Color hot, Color tc = {215,225,255,255}, int fs = 18)
{
    bool hov = Hovered(r);
    DrawRectangleRounded(r, 0.22f, 8, hov ? hot : bg);
    if (hov) DrawRectangleRoundedLinesEx(r, 0.22f, 8, 1.5f, C_ACCENT);
    int tw = MeasureText(lbl, fs);
    DrawText(lbl,
             (int)(r.x + r.width  / 2 - tw / 2),
             (int)(r.y + r.height / 2 - fs / 2),
             fs, tc);
    return hov && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

static void ButtonOff(Rectangle r, const char* lbl, int fs = 18) {
    DrawRectangleRounded(r, 0.22f, 8, C_SOFT_BROWN);
    int tw = MeasureText(lbl, fs);
    DrawText(lbl,
             (int)(r.x + r.width  / 2 - tw / 2),
             (int)(r.y + r.height / 2 - fs / 2),
             fs, C_TEXT_DIM);
}

// Radio. Returns true if clicked.
static bool Radio(int x, int y, const char* lbl, bool sel, bool disabled, int fs = 16)
{
    const int R = 6;
    Color cc = disabled ? C_TEXT_DIM : (sel ? C_ACCENT : C_BORDER);
    DrawCircle(x + R, y + R, (float)R, C_PANEL_ALT);
    DrawCircleLines(x + R, y + R, (float)R, cc);
    if (sel && !disabled) DrawCircle(x + R, y + R, (float)(R - 2), C_ACCENT);
    Color tc = disabled ? C_TEXT_DIM : (sel ? C_TEXT : Color{168, 178, 210, 255});
    DrawText(lbl, x + R * 2 + 6, y, fs, tc);
    if (disabled) return false;
    Rectangle hit = { (float)x, (float)y, 190.f, (float)(R * 2) };
    return Hovered(hit) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

// Section divider
static void Section(int x, int y, int w, const char* lbl) {
    DrawText(lbl, x, y, 20, C_TEXT_DIM);
    DrawLine(x, y + 20, x + w, y + 20, C_BORDER);
}

// Text input field
static void TextBox(Rectangle r, char* buf, int& len, bool& active, int maxCh, const char* placeholder = "")
{
    bool hov = Hovered(r);
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) active = hov;

    Color border = active ? C_ACCENT : (hov ? C_TEXT_DIM : C_BORDER);
    DrawRectangleRec(r, C_SOFT_BROWN);
    DrawRectangleLinesEx(r, 1.5f, border);

    if (active) {
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 126 && len < maxCh) {
                buf[len++] = (char)key;
                buf[len]   = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[--len] = '\0';
    }

    const char* disp = (len == 0 && !active) ? placeholder : buf;
    Color tc = (len == 0 && !active) ? C_TEXT_DIM : C_TEXT;
    DrawText(disp, (int)(r.x + 8), (int)(r.y + r.height / 2 - 7), 18, tc);

    if (active && ((int)(GetTime() * 2) % 2 == 0)) {
        int tw = MeasureText(buf, 18);
        DrawRectangle((int)(r.x + 8 + tw), (int)(r.y + 6),1, (int)(r.height - 12), C_ACCENT);
    }
}

// Horizontal divider line
static void Divider(int x, int y, int w) {
    DrawLine(x, y, x + w, y, C_BORDER);
}

static void DrawMoveSequencePanel(int x, int y, int w, int h, GuiState& gui){
    static float moveScroll = 0.f;

    Rectangle view = {(float)x,(float)y,(float)w,(float)h};

    DrawRectangleRounded(view, 0.18f, 6, C_PANEL_ALT);
    DrawRectangleLinesEx(view, 1.0f, C_BORDER);

    if (Hovered(view))
        moveScroll -= GetMouseWheelMove() * 24.f;
    

    static const Color dirColors[] = {
        {140,170,205,255},
        {205,145,130,255},
        {145,185,145,255},
        {220,190,120,255},
    };

    const int fs = 18;
    const int chW = MeasureText("R", fs) + 2;
    const int perRow = max(1, ((int)view.width - 18) / chW);

    int totalRows = (gui.result.path.size() + perRow - 1) / perRow;
    float contentHeight = totalRows * (fs + 4) + 8;
    float maxScroll = max(0.f, contentHeight - view.height);
    moveScroll = max(0.f, min(moveScroll, maxScroll));

    BeginScissorMode((int)view.x, (int)view.y, (int)view.width, (int)view.height);{
        int mx = x + 6;
        int drawY = y + 6 - (int)moveScroll;

        for (size_t i = 0; i < gui.result.path.size(); i++)
        {
            if ((int)i > 0 && (int)i % perRow == 0){
                mx = x + 6;
                drawY += fs + 4;
            }

            char d = gui.result.path[i].direction;

            Color tc = C_TEXT;

            if      (d == 'U') tc = dirColors[0];
            else if (d == 'D') tc = dirColors[1];
            else if (d == 'L') tc = dirColors[2];
            else if (d == 'R') tc = dirColors[3];

            if (gui.anim.currentStep > 0 &&
                (int)i == gui.anim.currentStep - 1)
            {
                DrawRectangleRounded(
                    {(float)(mx - 2), (float)(drawY - 1), (float)(chW + 1), (float)(fs + 3) }, 
                    0.25f, 4, Fade(C_ACCENT, 0.25f));
            }

            DrawText( string(1, d).c_str(), mx, drawY, fs, tc);
            mx += chW;
        }
    }

    EndScissorMode();

    // Scrollbar
    if (contentHeight > view.height)
    {
        float barH =max(24.f, view.height * (view.height / contentHeight));
        float barY = view.y + (moveScroll / maxScroll) * (view.height - barH);

        DrawRectangleRounded({view.x + view.width - 5, barY, 4, barH}, 1.0f, 4, Fade(C_TEXT_DIM, 0.55f));
    }
}

// ================= LEFT PANEL =================
void drawLeftPanel(GuiState& gui, Rectangle panel) {
    DrawRectangleRec(panel, C_PANEL);
    DrawLine((int)(panel.x + panel.width - 1), (int)panel.y,
             (int)(panel.x + panel.width - 1), (int)(panel.y + panel.height), C_BORDER);

    const int x = (int)panel.x + 14;
    const int w = (int)panel.width - 28;
    int y = (int)panel.y + 16;

    // ---- Header ----
    DrawText("ICE SLIDING", x, y, 30, C_ACCENT);
    DrawText("PUZZLE SOLVER", x, y + 30, 18, C_TEXT_DIM);
    DrawLine(x, y + 54, x + w, y + 54, C_BORDER);
    y += 60;

    // ---- File input ----
    Section(x, y, w, "INPUT FILE");
    y += 22;
    TextBox({(float)x, (float)y, (float)w, 30.f}, gui.fileInput, gui.fileInputLen, gui.fileInputActive, MAX_INPUT_CHARS, "contoh: test1.txt");
    y += 40;

    // ---- Algorithm ----
    Section(x, y, w, "ALGORITHM");
    y += 25;
    const char* algos[] = { "UCS", "GBFS", "A*" };
    for (int i = 0; i < 3; i++) {
        if (Radio(x, y, algos[i], gui.algoSelected == i, false))
            gui.algoSelected = i;
        y += 22;
    }
    y += 8;

    // ---- Heuristic ----
    bool heurOn = (gui.algoSelected != 0);
    Section(x, y, w, "HEURISTIC");
    y += 25;
    const char* heurs[] = { "H1 - Manhattan", "H2 - Euclidean", "H3 - Dijkstra" };
    for (int i = 0; i < 3; i++) {
        if (Radio(x, y, heurs[i], gui.heurSelected == i, !heurOn)) {
            if (heurOn) gui.heurSelected = i;
        }
        y += 22;
    }
    y += 16;

    // ---- RUN ----
    if (gui.fileInputLen > 0) {
        if (Button({(float)x,(float)y,(float)w, 36.f}, "RUN",
                   C_GREEN, C_GREEN_HOT))
            gui.runRequested = true;
    } else {
        ButtonOff({(float)x,(float)y,(float)w, 36.f}, "RUN");
    }
    y += 46;

    // ---- SAVE ----
    bool canSave = (gui.appState == AppState::RESULT_FOUND ||
                    gui.appState == AppState::RESULT_NOT_FOUND);
    if (canSave) {
        if (Button({(float)x,(float)y,(float)w, 36.f}, "SAVE RESULT",
                   C_BLUE, C_BLUE_HOT))
            gui.saveRequested = true;
    } else {
        ButtonOff({(float)x,(float)y,(float)w, 36.f}, "SAVE RESULT");
    }
    y += 50;

    // ---- Status ----
    Divider(x, y, w);
    y += 10;
    if (gui.appState == AppState::INVALID_BOARD) {
        DrawText("! Board tidak valid", x, y, 18, C_TEXT_ERR);
        y += 18;
        // show first ~30 chars of error
        if (!gui.errorMsg.empty()) {
            string e = gui.errorMsg;
            if ((int)e.size() > 28) e = e.substr(0, 28) + "...";
            DrawText(e.c_str(), x, y, 13, C_TEXT_ERR);
        }
    } else if (gui.appState == AppState::RESULT_NOT_FOUND) {
        DrawText("Solusi tidak ditemukan.", x, y, 18, C_TEXT_WARN);
    } else if (gui.appState == AppState::RESULT_FOUND) {
        DrawText("Solusi ditemukan!", x, y, 18, C_TEXT_OK);
    }
}


// ===================== BOARD PANEL =====================

static Node currentPlayerPos(const GuiState& gui) {
    if (!gui.boardLoaded) return {0, 0};
    if (gui.stepStates.empty()) return gui.graph.start;
    int step = gui.anim.currentStep;
    if (step < 0) step = 0;
    if (step >= (int)gui.stepStates.size())
        step = (int)gui.stepStates.size() - 1;
    return gui.stepStates[step].pos;
}

static int currentCheckpointProgress(const GuiState& gui) {
    if (!gui.boardLoaded || gui.stepStates.empty()) return 0;
    int step = gui.anim.currentStep;
    if (step < 0) step = 0;
    if (step >= (int)gui.stepStates.size())
        step = (int)gui.stepStates.size() - 1;
    return gui.stepStates[step].nextCheckpoint;
}

void drawBoardPanel(GuiState& gui, Rectangle panel, float dt) {
    const int px  = (int)panel.x;
    const int py  = (int)panel.y;
    const int pw  = (int)panel.width;
    const int ph  = (int)panel.height;
    const int ctrlH = 105;
    const int boardAreaH = ph - ctrlH;

    DrawRectangleRec(panel, C_BG);
    // side borders
    DrawLine(px, py, px,py + ph, C_BORDER);
    DrawLine(px + pw, py, px + pw, py + ph, C_BORDER);

    // ---- Placeholder when no board loaded ----
    if (!gui.boardLoaded) {
        int gs = 28;
        for (int i = 0; i < 9; i++) for (int j = 0; j < 9; j++) {
            DrawRectangle(px + pw/2 - 126 + j*gs, py + boardAreaH/2 - 126 + i*gs,
                          gs - 1, gs - 1, C_SOFT_BROWN);
        }
        const char* msg = "HELLO WORLD";
        int tw = MeasureText(msg, 24);
        DrawText(msg, px + pw/2 - tw/2, py + boardAreaH/2 - 9, 24, C_TEXT_DIM);
    } else {
        // ---- Board ----
        const int rows = gui.board.rows;
        const int cols = gui.board.cols;
        const int padX = 22, padY = 22;
        const int availW = pw - padX * 2;
        const int availH = boardAreaH - padY * 2;
        int bs = min(availW / cols, availH / rows);
        bs = max(bs, 6);

        const int bw = bs * cols;
        const int bh = bs * rows;
        const int ox = px + padX + (availW - bw) / 2;
        const int oy = py + padY + (availH - bh) / 2;

        // Path highlight tiles for current step
        vector<pair<int,int>> pathTiles;
        if (gui.appState == AppState::RESULT_FOUND &&
            !gui.result.path.empty() &&
            gui.anim.currentStep > 0 &&
            gui.anim.currentStep <= (int)gui.result.path.size())
        {
            pathTiles = gui.result.path[gui.anim.currentStep - 1].tiles;
        }

        Node player = currentPlayerPos(gui);
        int checkpointProgress = currentCheckpointProgress(gui);

        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                int gx = ox + c * bs;
                int gy = oy + r * bs;
                char ch = gui.board.grid[r][c];
                char visibleCh = ch;
                if (ch >= '0' && ch <= '9' && (ch - '0') < checkpointProgress)
                    visibleCh = '*';

                // Base tile colour
                Color bg;
                if      (visibleCh == 'X')                 bg = C_TILE_ROCK;
                else if (visibleCh == 'L')                 bg = C_TILE_LAVA;
                else if (visibleCh == 'O')                 bg = C_TILE_GOAL;
                else if (visibleCh == 'Z')                 bg = C_TILE_START;
                else if (visibleCh >= '0' && visibleCh <= '9') bg = C_TILE_CP;
                else                                bg = C_TILE_EMPTY;

                DrawRectangle(gx, gy, bs, bs, bg);

                // Traversal overlay (skip player tile)
                bool onPath = false;
                for (auto& t : pathTiles)
                    if (t.first == r && t.second == c) { onPath = true; break; }
                bool isPlayer = (r == player.row && c == player.col);
                if (onPath && !isPlayer)
                    DrawRectangle(gx, gy, bs, bs, C_TILE_TRAV);

                // Grid line
                DrawRectangleLines(gx, gy, bs, bs, {0, 0, 0, 70});

                // Cell text label
                if (bs >= 16) {
                    int fs = max(9, min(bs / 2, 28));
                    string lbl;
                    if      (visibleCh == 'O') lbl = "O";
                    else if (visibleCh == 'Z') lbl = "Z";
                    else if (visibleCh >= '0' && visibleCh <= '9') lbl = string(1, visibleCh);
                    else if (visibleCh == 'L') lbl = "L";
                    if (!lbl.empty()) {
                        int tw = MeasureText(lbl.c_str(), fs);
                        DrawText(lbl.c_str(),
                                 gx + bs/2 - tw/2, gy + bs/2 - fs/2,
                                 fs, {20, 20, 20, 200});
                    }
                    // Cost hint (bottom-right, only when block big enough)
                    if (bs >= 32 && visibleCh != 'X' && visibleCh != 'L') {
                        string cs = to_string(gui.board.cost[r][c]);
                        int cw = MeasureText(cs.c_str(), 12);
                        DrawText(cs.c_str(),
                                 gx + bs - cw - 2, gy + bs - 12,
                                 9, {55, 55, 55, 170});
                    }
                }
            }
        }

        // Draw row/col indices when large enough
        if (bs >= 24) {
            int idx_fs = max(9, min(bs / 3, 18));
            for (int c = 0; c < cols; c++) {
                string s = to_string(c);
                int tw = MeasureText(s.c_str(), idx_fs);
                DrawText(s.c_str(),
                         ox + c*bs + bs/2 - tw/2, oy - idx_fs - 3,
                         idx_fs, C_TEXT_DIM);
            }
            for (int r = 0; r < rows; r++) {
                string s = to_string(r);
                DrawText(s.c_str(), ox - MeasureText(s.c_str(), idx_fs) - 4, oy + r*bs + bs/2 - idx_fs/2, idx_fs, C_TEXT_DIM);
            }
        }

        // Draw player dot
        {
            int rad = max(4, bs / 3);
            DrawCircle(ox + player.col * bs + bs/2, oy + player.row * bs + bs/2, (float)rad, C_PLAYER);
            DrawCircleLines(ox + player.col * bs + bs/2, oy + player.row * bs + bs/2, (float)rad, WHITE);
        }
    }

    // ---- Animate update ----
    if (gui.appState == AppState::RESULT_FOUND &&
        gui.anim.playing &&
        !gui.result.path.empty())
    {
        float spd = ANIM_SPEEDS[gui.anim.speedIndex];
        gui.anim.timer += dt;
        if (gui.anim.timer >= 1.0f / spd) {
            gui.anim.timer = 0.f;
            int total = (int)gui.result.path.size();
            if (gui.anim.currentStep < total) {
                gui.anim.currentStep++;
            } else {
                gui.anim.playing = false;
            }
        }
    }

    // ======================================================
    //  CONTROL BAR
    // ======================================================
    int cy = py + ph - ctrlH;
    DrawRectangle(px, cy, pw, ctrlH, C_PANEL);
    DrawLine(px, cy, px + pw, cy, C_BORDER);

    bool hasResult = (gui.appState == AppState::RESULT_FOUND && !gui.result.path.empty());
    int totalSteps = hasResult ? (int)gui.result.path.size() : 0;

    // Step / direction label
    {
        string stepStr, dirStr;
        if (hasResult) {
            stepStr = "Step " + to_string(gui.anim.currentStep)
                    + " / " + to_string(totalSteps);
            if (gui.anim.currentStep > 0 &&
                gui.anim.currentStep <= (int)gui.result.path.size())
            {
                char d = gui.result.path[gui.anim.currentStep - 1].direction;
                string dn;
                if      (d == 'U') dn = "UP";
                else if (d == 'D') dn = "DOWN";
                else if (d == 'L') dn = "LEFT";
                else               dn = "RIGHT";
                dirStr = "[ " + dn + " ]";
            }
        } else {
            stepStr = "Step 0 / 0";
        }
        int tw = MeasureText(stepStr.c_str(), 18);
        DrawText(stepStr.c_str(), px + pw/2 - tw/2, cy + 6, 18, C_TEXT_DIM);
        if (!dirStr.empty()) {
            int dw = MeasureText(dirStr.c_str(), 18);
            DrawText(dirStr.c_str(), px + pw/2 - dw/2, cy + 22, 18, C_TEXT_ACC);
        }
    }

    // Nav buttons: |<  <  play/pause  >  >|
    const int bw2 = 44, bh2 = 30;
    const int gap  = 7;
    const int totalBW = bw2 * 5 + gap * 4;
    int bx = px + pw / 2 - totalBW / 2;
    const int by2 = cy + 42;

    auto navBtn = [&](const char* lbl, bool enabled, auto action) {
        if (enabled) {
            if (Button({(float)bx,(float)by2,(float)bw2,(float)bh2},
                       lbl, C_PANEL, C_BG, C_TEXT, 14))
                action();
        } else {
            ButtonOff({(float)bx,(float)by2,(float)bw2,(float)bh2}, lbl, 14);
        }
        bx += bw2 + gap;
    };

    // |<
    navBtn("|<", hasResult, [&](){
        gui.anim.currentStep = 0;
        gui.anim.playing = false;
        gui.anim.timer = 0;
    });
    // <
    navBtn("<", hasResult && gui.anim.currentStep > 0, [&](){
        gui.anim.currentStep--;
        gui.anim.playing = false;
        gui.anim.timer = 0;
    });
    // play / pause
    {
        const char* pl = gui.anim.playing ? "||" : "I>";
        navBtn(pl, hasResult, [&](){
            if (gui.anim.currentStep >= totalSteps) {
                gui.anim.currentStep = 0;
                gui.anim.timer = 0;
            }
            gui.anim.playing = !gui.anim.playing;
        });
    }
    // >
    navBtn(">", hasResult && gui.anim.currentStep < totalSteps, [&](){
        gui.anim.currentStep++;
        gui.anim.playing = false;
        gui.anim.timer = 0;
    });
    // >|
    navBtn(">|", hasResult, [&](){
        gui.anim.currentStep = totalSteps;
        gui.anim.playing = false;
        gui.anim.timer = 0;
    });

    // Speed buttons
    {
        int sy = by2 + bh2 + 7;
        int labelW = MeasureText("Speed:", 14);
        int buttonsW = 0;

        for (int i = 0; i < NUM_SPEEDS; i++) {
            buttonsW += MeasureText(SPEED_LABELS[i], 14) + 14;

            if (i < NUM_SPEEDS - 1)
                buttonsW += 4;
        }
        
        int totalW = labelW + 2 + buttonsW; //total width termasuk Speed:
        int sx = px + pw/2 - totalW/2;
        DrawText("Speed:", sx, sy + 2, 14, C_TEXT_DIM);
        sx += labelW + 2;

        for (int i = 0; i < NUM_SPEEDS; i++) {
            int sw = MeasureText(SPEED_LABELS[i], 14) + 14;
            Rectangle sr = {(float)sx, (float)sy, (float)sw, 18.f};
            bool sel = (gui.anim.speedIndex == i);

            DrawRectangleRounded(sr, 0.4f, 6, sel ? C_ACCENT : C_SOFT_BROWN);
            DrawText( SPEED_LABELS[i], sx + 7, sy + 3, 12, sel ? C_BG : C_TEXT_DIM);

            if (!sel && Hovered(sr) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) gui.anim.speedIndex = i;
            sx += sw + 4;
        }
    }
}

// ======================================================
//  RIGHT PANEL
// ======================================================
void drawRightPanel(GuiState& gui, Rectangle panel) {
    DrawRectangleRec(panel, C_PANEL);
    DrawLine((int)panel.x, (int)panel.y,
             (int)panel.x, (int)(panel.y + panel.height), C_BORDER);

    const int x = (int)panel.x + 14;
    const int w = (int)panel.width - 44;
    int y = (int)panel.y + 16;

    // ---- Header ----
    DrawText("RESULTS", x, y, 18, C_TEXT_ACC);
    Divider(x, y + 22, w);
    y += 34;

    // ---- Idle / Error ----
    if (gui.appState == AppState::IDLE) {
        DrawText("Kamu belum jalanin apapun.", x, y, 18, C_TEXT_DIM);
        return;
    }
    if (gui.appState == AppState::INVALID_BOARD) {
        DrawText("Board tidak valid, periksa file input.", x, y, 18, C_TEXT_ERR);
        return;
    }

    // ---- Algorithm ----
    const char* algoN[] = { "UCS", "GBFS", "A*" };
    const char* heurN[] = { "H1 (Manhattan)", "H2 (Euclidean)", "H3 (Dijkstra)" };
    Section(x, y, w, "CONFIGURATION");
    y += 24;
    string algoStr = algoN[gui.algoSelected];
    if (gui.algoSelected != 0) algoStr += " + " + string(heurN[gui.heurSelected]);
    DrawText("Algorithm :", x, y, 18, C_TEXT_DIM);
    DrawText(algoStr.c_str(), x + 98, y, 18, C_TEXT);
    y += 24;

    // ---- Key metrics ----
    Section(x, y, w, "METRICS");
    y += 24;

    // Status
    DrawText("Status    :", x, y, 18, C_TEXT_DIM);
    if (gui.appState == AppState::RESULT_FOUND)
        DrawText("DITEMUKAN", x + 84, y, 18, C_TEXT_OK);
    else
        DrawText("TIDAK DITEMUKAN", x + 84, y, 18, C_TEXT_ERR);
    y += 18;

    // Execution time
    {
        ostringstream oss;
        oss << fixed << setprecision(2) << gui.executionTimeMs << " ms";
        DrawText("Waktu     :", x, y, 18, C_TEXT_DIM);
        DrawText(oss.str().c_str(), x + 84, y, 18, C_TEXT);
        y += 18;
    }

    // Iterations
    DrawText("Iterasi    :", x, y, 18, C_TEXT_DIM);
    DrawText(to_string(gui.result.iterations).c_str(), x + 84, y, 18, C_TEXT);
    y += 18;

    if (gui.appState == AppState::RESULT_NOT_FOUND) {
        y += 6;
        DrawText("Tidak ada jalur yang valid menuju goal.", x, y, 18, C_TEXT_WARN);
        return;
    }

    // Path cost + steps
    DrawText("Total Cost:", x, y, 18, C_TEXT_DIM);
    DrawText(to_string((int)gui.result.totalCost).c_str(), x + 92, y, 18, C_TEXT);
    y += 18;

    DrawText("Langkah  :", x, y, 18, C_TEXT_DIM);
    DrawText(to_string(gui.result.path.size()).c_str(), x + 87, y, 18, C_TEXT);
    y += 22;

    // ---- Move sequence ----
    Divider(x, y, w);
    y += 8;
    
    Section(x, y, w, "URUTAN GERAKAN");

    y += 24;

    DrawMoveSequencePanel(x, y, w, 150, gui);

    y += 154;

    // ---- Current step detail ----
    Divider(x, y, w);
    y += 8;
    Section(x, y, w, "DETAIL LANGKAH");
    y += 24;

    int showStep = gui.anim.currentStep;
    // If at step 0 but path exists, preview first step
    if (showStep == 0 && !gui.result.path.empty()) showStep = 1;

    if (showStep >= 1 && showStep <= (int)gui.result.path.size()) {
        const Edge& e = gui.result.path[showStep - 1];

        // Cumulative cost
        float cumCost = 0;
        for (int i = 0; i < showStep; i++) cumCost += gui.result.path[i].cost;

        string dn;
        if      (e.direction == 'U') dn = "ATAS (U)";
        else if (e.direction == 'D') dn = "BAWAH (D)";
        else if (e.direction == 'L') dn = "KIRI (L)";
        else                         dn = "KANAN (R)";

        DrawText(TextFormat("Step  : %d / %d", showStep, (int)gui.result.path.size()),
                 x, y, 18, C_TEXT_DIM); y += 18;
        DrawText(TextFormat("Arah  : %s", dn.c_str()), x, y, 18, C_TEXT); y += 18;
        DrawText(TextFormat("Dari  : (%d, %d)", e.curr.row, e.curr.col),
                 x, y, 18, C_TEXT); y += 18;
        DrawText(TextFormat("Ke    : (%d, %d)", e.next.row, e.next.col),
                 x, y, 18, C_TEXT); y += 18;
        DrawText(TextFormat("Cost  : %d", e.cost), x, y, 18, C_TEXT); y += 18;
        {
            ostringstream oss;
            oss << fixed << setprecision(0) << cumCost;
            DrawText(("Total Cost: " + oss.str()).c_str(),
                     x, y, 18, C_TEXT_ACC);
        }
    } else {
        DrawText("Tekan > atau Play untuk mulai.", x, y, 18, C_TEXT_DIM);
    }
}


//  =================== SAVE RESULT ===================
void saveResult(const GuiState& gui) {
    // Build filename with timestamp
    time_t now = time(nullptr);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y%m%d_%H%M%S", localtime(&now));

    const char* algoTag[] = { "UCS", "GBFS", "AStar" };
    string fname = string("result_") + algoTag[gui.algoSelected] + "_" + ts + ".txt";

    // Try output directories first, then current dir.
    auto tryOpen = [&](const filesystem::path& dir) -> ofstream {
        error_code ec;
        filesystem::create_directories(dir, ec);
        return ofstream(dir / fname);
    };

    ofstream f = tryOpen(filesystem::path("..") / "test" / "output");
    if (!f.is_open()) f = tryOpen(filesystem::path("test") / "output");
    if (!f.is_open()) f = tryOpen("output");
    if (!f.is_open()) f.open(fname);   // current dir fallback
    if (!f.is_open()) return;

    const char* algoNames[] = { "UCS", "GBFS", "A*" };
    const char* heurNames[] = { "H1 (Manhattan)", "H2 (Euclidean)", "H3 (Dijkstra)" };

    f << "=========================================\n";
    f << "               RESULT LOG\n";
    f << "=========================================\n\n";
    f << "File       : " << gui.fileInput << "\n";
    f << "Algorithm  : " << algoNames[gui.algoSelected];
    if (gui.algoSelected != 0) f << " + " << heurNames[gui.heurSelected];
    f << "\n";
    f << fixed << setprecision(2);
    f << "Waktu      : " << gui.executionTimeMs << " ms\n";
    f << "Iterasi    : " << gui.result.iterations << "\n";

    if (!gui.result.found) {
        f << "Status     : SOLUSI TIDAK DITEMUKAN\n";
    } else {
        f << "Status     : SOLUSI DITEMUKAN\n";
        f << "Total Cost : " << (int)gui.result.totalCost << "\n";
        f << "Langkah    : " << gui.result.path.size() << "\n\n";

        // Move sequence
        f << "Urutan Gerakan : ";
        for (const Edge& e : gui.result.path) f << e.direction;
        f << "\n\n";

        // Step-by-step detail
        f << "Detail Langkah:\n";
        float cumCost = 0;
        for (size_t i = 0; i < gui.result.path.size(); i++) {
            const Edge& e = gui.result.path[i];
            cumCost += e.cost;
            string dn;
            if      (e.direction == 'U') dn = "UP";
            else if (e.direction == 'D') dn = "DOWN";
            else if (e.direction == 'L') dn = "LEFT";
            else                         dn = "RIGHT";
            f << "  Step " << setw(3) << (i + 1) << " : "
              << dn << "\t"
              << "(" << e.curr.row << "," << e.curr.col << ")"
              << " -> "
              << "(" << e.next.row << "," << e.next.col << ")"
              << "  cost=" << e.cost
              << "  kumulatif=" << (int)cumCost
              << "\n";
        }
    }
    f << "\n=========================================\n";
    f.close();
}