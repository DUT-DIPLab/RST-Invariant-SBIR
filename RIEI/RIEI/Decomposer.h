#pragma once

#include <vector>
#include "Sketch.h"

class Decomposer
{
public:
    Decomposer();
    ~Decomposer();

    std::vector<Sketch<>> decompose(Sketch<>& sketch);
};

