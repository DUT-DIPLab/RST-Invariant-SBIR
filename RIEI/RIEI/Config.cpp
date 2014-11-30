#include <cstdio>
#include <cstring>
#include "Config.h"

Config::Config()
{
}

Config* Config::instance()
{
    static Config config;
    return &config;
}

Config::~Config()
{
}

void Config::read(const char* fileName)
{
    char buffer[1024];
    FILE* file = fopen(fileName, "r");
    while (fscanf(file, "%s", buffer) != EOF)
    {
        if (strcmp(buffer, "canny_low") == 0)
        {
            fscanf(file, "%d", &cannyLow);
        }
        else if (strcmp(buffer, "canny_high") == 0)
        {
            fscanf(file, "%d", &cannyHigh);
        }
        else if (strcmp(buffer, "down_sample_ratio") == 0)
        {
            fscanf(file, "%lf", &downSampleRatio);
        }
        else if (strcmp(buffer, "down_sample_thres") == 0)
        {
            fscanf(file, "%d", &downSampleThres);
        }
        else if (strcmp(buffer, "cluster_min_size") == 0)
        {
            fscanf(file, "%d", &clusterMinSize);
        }
        else if (strcmp(buffer, "sketch_width") == 0)
        {
            fscanf(file, "%d", &sketchWidth);
        }
        else if (strcmp(buffer, "sketch_height") == 0)
        {
            fscanf(file, "%d", &sketchHeight);
        }
    }
    fclose(file);
}