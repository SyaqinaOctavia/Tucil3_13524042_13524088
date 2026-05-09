#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "board.hpp"
using namespace std;

void loadFile(string filename, Board* mainBoard);
void saveResult(const GuiState& gui);