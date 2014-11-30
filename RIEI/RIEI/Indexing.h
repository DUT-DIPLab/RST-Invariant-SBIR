#pragma once

#include <vector>
#include "Task.h"
#include "Sketch.h"
using namespace std;

struct Index
{
    int id;
    int part;
    bool operator <(const Index& index) const
    {
        if (id == index.id)
        {
            return part < index.part;
        }
        return id < index.id;
    }
};

struct Score
{
    Index index;
    double score;
    bool operator <(const Score& s) const
    {
        return score > s.score;
    }
};

class Indexing
{
public:
    Indexing();
    ~Indexing();

    vector<vector<vector<bool>>> generateHitmap(const Sketch& sketch);
    vector<vector<vector<bool>>> readHitmap(const char* filePath);
    void writeHitmap(const vector<vector<vector<bool>>>& hitmap, const char* filePath);

    void generateIndexing(const Task& task);
    vector<Score> query(const Task& task, const Sketch& sketch);

private:
    vector<vector<vector<vector<Index>>>> _index;

    vector<vector<vector<bool>>> calcAngle(const Sketch& sketch);
};

