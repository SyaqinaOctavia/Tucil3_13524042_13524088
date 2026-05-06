#pragma once

#include <string>
#include "graph.hpp"

using namespace std;

class HeuristikCost {
public:
    virtual float cost() = 0;
};

class ManhattanCost : public HeuristikCost{
public:
    float cost() override;
};

