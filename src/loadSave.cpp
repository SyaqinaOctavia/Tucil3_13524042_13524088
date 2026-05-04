#include "loadSave.hpp"

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
    file.ignore(); 

    vector<char> rowC;
    vector<int> rowI;

    while(i < mainBoard->rows){
        file.get(ch);
        if (ch == '\n') {
            mainBoard->grid.push_back(rowC);
            rowC.clear();
            i++;
            j = 0;        
        }else {
            rowC.push_back(ch);
            j++;
            if(j > mainBoard->cols){
                cout << "board tidak valid\n";
                break;
            }
        }
    }

    string line;

    i = 0;
    while (i < mainBoard->rows && getline(file, line)) {
        vector<int> rowI;
        stringstream ss(line);
        string num;
        while (ss >> num) {
            rowI.push_back(stoi(num));
        }
        if ((int)rowI.size() < mainBoard->cols) {
            cout << "cost tidak valid di baris " << i << "\n";
            break;
        }
        mainBoard->cost.push_back(rowI);
        i++;
    }
}