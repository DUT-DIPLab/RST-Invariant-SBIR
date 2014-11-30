#pragma once

#include "Sketch.h"

class Preprocesser
{
public:
    Preprocesser();
    ~Preprocesser();

    Sketch<> preprocess(const char* filePath);

    IplImage* generateEdges(const char* filePath);
    Sketch<> cutOutSketch(IplImage* canny);

    void sketchThinning(Sketch<>& sketch);
};

