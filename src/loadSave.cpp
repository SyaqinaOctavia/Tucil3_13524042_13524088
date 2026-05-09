#include "include/loadSave.hpp"
#include <limits>
#include <filesystem>

void loadFile(string filename, Board* mainBoard){
    namespace fs = std::filesystem;

    ifstream file;
    const fs::path inputName(filename);
    const fs::path candidates[] = {
        inputName,
        fs::path("test") / "input" / inputName,
        fs::path("..") / "test" / "input" / inputName,
        fs::path("..") / ".." / "test" / "input" / inputName
    };

    for (const fs::path& candidate : candidates) {
        file.open(candidate);
        if (file.is_open()) break;
        file.clear();
    }

    if (!file.is_open()) {
        cout << "Gagal membuka file: " << filename << "\n";
        return;
    }
    char ch;
    int i = 0, j = 0;

    file >> mainBoard->rows >> mainBoard->cols;
    file.ignore(numeric_limits<streamsize>::max(), '\n');

    vector<char> rowC;
    vector<int> rowI;
    
    while(i < mainBoard->rows){
        file.get(ch);
        
        if (ch == '\r') continue;
        if (ch == '\n') {
            if (j != mainBoard->cols) {
                cout << "Baris " << i << " panjangnya " << j 
                     << ", harusnya " << mainBoard->cols << "\n";
                return;
            }

            mainBoard->grid.push_back(rowC);
            rowC.clear();
            i++;
            j = 0;        
        }else {
            if(j >= mainBoard->cols){
                cout << "board tidak valid\n";
                break;
            }

            rowC.push_back(ch);

            if(isdigit(ch)){
                int num = ch - '0';
                mainBoard->checkpoints[num] = Node{i, j}; // save checkpoint
            }
            if(ch == 'O') mainBoard->goal = Node{i, j}; // save target tile
            if(ch == 'Z') mainBoard->start = Node{i, j}; // save target tile

            j++;
        }
    }

    string line;

    i = 0;
    while (i < mainBoard->rows && getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        stringstream ss(line);
        int num;
        while (ss >> num) {
            rowI.push_back(num);
        }
        if ((int)rowI.size() < mainBoard->cols) {
            cout << "cost tidak valid di baris " << i << "\n";
            return;
        }
        mainBoard->cost.push_back(rowI);
        rowI.clear();
        i++;
    }
}

//  =================== SAVE RESULT ===================
void saveResult(const GuiState& gui){
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