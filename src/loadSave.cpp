#include "loadSave.hpp"
#include <limits>

void loadFile(string filename, Board* mainBoard){
    string defaultPath = "../data/input/";
    ifstream file(defaultPath + filename);

    if (!file.is_open()) {
        cout << "Gagal membuka file!\n";
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