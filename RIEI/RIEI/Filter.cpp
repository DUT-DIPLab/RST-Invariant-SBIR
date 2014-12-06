#include "Filter.h"

Filter::Filter()
{
}

Filter::~Filter()
{
}

vector<vector<double>> Filter::filter(const Sketch& sketch, vector<vector<double>>& mask)
{
    int maskLen = (int)mask.size();
    int shift = - maskLen / 2;
    vector<vector<double>> filtered(sketch.row(), vector<double>(sketch.col(), 0.0));
    for (int r = 0; r < sketch.row(); ++r)
    {
        for (int c = 0; c < sketch.col(); ++c)
        {
            for (int i = 0; i < maskLen; ++i)
            {
                for (int j = 0; j < maskLen; ++j)
                {
                    filtered[r][c] += mask[i][j] * sketch.safeGet(r + i + shift, c + j + shift);
                }
            }
        }
    }
    return filtered;
}

vector<vector<double>> Filter::filterSobelHorizon(const Sketch& sketch)
{
    vector<vector<double>> sobel = { { -1, -2, -1 }, { 0, 0, 0 }, { 1, 2, 1 } };
    return filter(sketch, sobel);
}

vector<vector<double>> Filter::filterSobelVertical(const Sketch& sketch)
{
    vector<vector<double>> sobel = { { -1, 0, 1 }, { -2, 0, 2 }, { -1, 0, 1 } };
    return filter(sketch, sobel);
}