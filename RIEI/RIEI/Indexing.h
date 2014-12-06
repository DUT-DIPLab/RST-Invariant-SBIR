#pragma once

#include <vector>
#include "Task.h"
#include "Sketch.h"
using namespace std;

struct Index
{
    int id : 16;
    int x : 8;
    int y : 8;

    bool operator <(const Index& index) const
    {
        if (id == index.id)
        {
            return x < index.x;
        }
        return y < index.y;
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
};

