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
        else if (strcmp(buffer, "sketch_side_length") == 0)
        {
            fscanf(file, "%d", &sketchSideLength);
        }
        else if (strcmp(buffer, "partition_num") == 0)
        {
            fscanf(file, "%d", &partitionNum);
        }
        else if (strcmp(buffer, "candidate_num") == 0)
        {
            fscanf(file, "%d", &candidateNum);
        }
        else if (strcmp(buffer, "patch_length") == 0)
        {
            fscanf(file, "%d", &patchLength);
        }
        else if (strcmp(buffer, "shift_length") == 0)
        {
            fscanf(file, "%d", &shiftLength);
        }
        else if (strcmp(buffer, "grid_num") == 0)
        {
            fscanf(file, "%d", &gridNum);
        }
        else if (strcmp(buffer, "bin_num") == 0)
        {
            fscanf(file, "%d", &binNum);
        }
        else if (strcmp(buffer, "sh_param") == 0)
        {
            fscanf(file, "%s", buffer);
            shParam.readParam(buffer);
        }
    }
    fclose(file);
}