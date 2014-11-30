#pragma once

#include <vector>
#include <string>

struct ImageInfo
{
    std::string name;
    std::string path;
};

struct Task
{
public:
    Task();
    ~Task();

    void read(const char* filePath);

    int datasetNum;
    int queryNum;
    bool queryIsSketch;
    std::vector<ImageInfo> datasets;
    std::vector<ImageInfo> queries;
};

