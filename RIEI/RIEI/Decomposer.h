#pragma once

#include <vector>
#include "Sketch.h"
using namespace std;

class Decomposer
{
public:
    Decomposer();
    ~Decomposer();

    vector<Sketch> decompose(const Sketch& sketch) const;
};

