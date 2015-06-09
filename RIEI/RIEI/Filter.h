#pragma once

#include "Sketch.h"

class Filter
{
public:
    Filter();
    ~Filter();

    vector<vector<double>> filter(const Sketch& sketch, vector<vector<double>>& mask);
    vector<vector<double>> filterSobelHorizon(const Sketch& sketch);
    vector<vector<double>> filterSobelVertical(const Sketch& sketch);
};

