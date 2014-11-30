#pragma once

#include <vector>
#include "Task.h"
#include "Sketch.h"
using namespace std;

struct Index
{
    int id;
    int part;
};

struct Score
{
    Index index;
    double score;
};

class Indexing
{
public:
    Indexing();
    ~Indexing();

    void generateIndexing(const Task& task);
    vector<Score> query(const Sketch& sketch);

private:
    vector<vector<vector<vector<Index>>>> _index;

    vector<vector<vector<bool>>> calcAngle(const Sketch& sketch);
    vector<vector<vector<bool>>> generateHitmap(const Sketch& sketch);
};

