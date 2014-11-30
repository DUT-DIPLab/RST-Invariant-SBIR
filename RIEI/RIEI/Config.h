#pragma once

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
    int clusterMinSize;
    int sketchSideLength;
    int hitmapRadius;
    int angleBinNum;
    int partitionNum;
    int candidateNum;

private:
    Config();
};

