#pragma once

#include <vector>
#include "Task.h"
#include "Sketch.h"
using namespace std;

struct Index
{
    int val;

    Index(int id = 0, int part = 0)
    {
        val = (id << 5) + part;
    }

    inline int id() const
    {
        return val >> 5;
    }

    inline int part() const
    {
        return val & ((1 << 5) - 1);
    }

    bool operator <(const Index& index) const
    {
        if (id() == index.id())
        {
            return part() < index.part();
        }
        return id() < index.id();
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

    vector<Score> query(const Task& task, const vector<vector<vector<vector<Index>>>>& indexs, const Sketch& sketch);

private:
    vector<vector<vector<bool>>> calcAngle(const Sketch& sketch);
};

