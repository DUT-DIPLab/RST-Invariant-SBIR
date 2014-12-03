#include <cmath>
#include "Config.h"
#include "Preprocesser.h"
#include "Decomposer.h"

const double PI = acos(-1.0);

Decomposer::Decomposer()
{
}

Decomposer::~Decomposer()
{
}

vector<Sketch> Decomposer::decompose(const Sketch& sketch) const
{
    Config* config = Config::instance();
    int radius = config->sketchSideLength >> 1;
    double halfTheta = PI / config->partitionNum;
    int halfWidth = (int)(radius * sin(halfTheta));
    int width = (halfWidth << 1) + 1;
    int height = (int)(radius * cos(halfTheta));
    int center = radius;
    vector<Sketch> parts(8, Sketch(height, width));
    Preprocesser processer;
    for (int k = 0; k < config->partitionNum; ++k)
    {
        for (int y = 0; y < height; ++y)
        {
            for (int x = -halfWidth; x <= halfWidth; ++x)
            {
                int oy = center - y;
                int ox = center + x;
                double rad = sqrt(x * x + y * y);
                double ang = atan2(y, x) - (k << 1) * halfTheta;
                double ry = center - rad * sin(ang);
                double rx = center + rad * cos(ang);
                parts[k][height - y - 1][x + halfWidth] = sketch.bilinearInterpolate(ry, rx) > 0.1;
            }
        }
        processer.sketchThinning(parts[k]);
    }
    return parts;
}