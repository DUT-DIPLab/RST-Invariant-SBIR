#pragma once

#include "ShParam.h"

struct Config
{
public:
    static Config* instance();
    ~Config();

    void read(const char* fileName);

    int cannyLow;
    int cannyHigh;
    double downSampleRatio;
    int downSampleThres;
    int sketchSideLength;
    int partitionNum;
    int candidateNum;
    int patchLength;
    int shiftLength;
    int gridNum;
    int binNum;
    ShParam shParam;

    double flipCost;
    double rotateCost;
    double varianceCost;
    double shiftCost;

    int radius;
    int partWidth;
    int partHeight;

private:
    Config();
};

