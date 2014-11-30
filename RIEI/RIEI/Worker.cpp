#include <cstdio>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"
#include "Decomposer.h"
#include "Indexing.h"
#include "Worker.h"

Worker::Worker()
{
}

Worker::~Worker()
{
}

void Worker::work(const char* filePath)
{
    Task task;
    task.read(filePath);
    work(task);
}

void Worker::work(const Task& task)
{
    Config* config = Config::instance();
    int parNum = config->partitionNum;
    config->radius = config->sketchSideLength >> 1;
    double halfTheta = acos(-1.0) / parNum;
    int halfWidth = (int)(config->radius * sin(halfTheta));
    config->partWidth = (halfWidth << 1) + 1;
    config->partHeight = (int)(config->radius * cos(halfTheta));

    char buffer[1024];
    Preprocesser preprocesser;
    Decomposer decomposer;
    Indexing indexing;
    for (int i = 0; i < task.datasetNum; ++i)
    {
        Sketch sketch = preprocesser.preprocess(task.datasets[i].path.c_str());
        sprintf(buffer, "sketches/%s.jpg", task.datasets[i].name.c_str());
        sketch.write(buffer);
        auto parts = decomposer.decompose(sketch);
        for (int j = 0; j < parNum; ++j)
        {
            auto hitmap = indexing.generateHitmap(parts[j]);
            sprintf(buffer, "hitmaps/%s_%d.hitmap", task.datasets[i].name.c_str(), j);
            indexing.writeHitmap(hitmap, buffer);
        }
    }
}