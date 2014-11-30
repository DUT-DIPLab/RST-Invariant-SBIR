#pragma once

class Preprocesser
{
public:
    Preprocesser();
    ~Preprocesser();

    void generateSketch(const char* input, const char* output);
    void cutOutSketch(const char* input, const char* output);
    void sketchThinning(const char* input, const char* output);
};

