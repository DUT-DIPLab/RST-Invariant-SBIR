#pragma once

#include <opencv2/opencv.hpp>
#include "Sketch.h"

class Preprocesser
{
public:
    Preprocesser();
    ~Preprocesser();

    Sketch preprocess(const char* filePath);

    Sketch generateEdges(const char* filePath);
    Sketch cutOutSketch(const Sketch&& edge);
};

