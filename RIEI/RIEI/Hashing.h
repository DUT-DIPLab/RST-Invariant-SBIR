#pragma once

#include "ShParam.h"
#include "Sketch.h"

class Hashing
{
public:
    Hashing();
    ~Hashing();

    vector<char> hashing(vector<double>& feature);
    vector<vector<char>> hashing(const Sketch& sketch);

    vector<vector<char>> read(const char* fileName);
    void write(const char* fileName, vector<vector<char>>& bits);
};

