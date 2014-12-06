#include <cmath>
#include <fstream>
#include <algorithm>
#include "Config.h"
#include "Filter.h"
#include "Hashing.h"

const double PI = acos(-1.0);

Hashing::Hashing()
{
}

Hashing::~Hashing()
{
}

vector<char> Hashing::hashing(vector<double>& feature)
{
    Config* config = Config::instance();
    ShParam& shParam = config->shParam;

    vector<double> x(shParam.bitNum(), 0.0);
    for (int i = 0; i < shParam.bitNum(); ++i)
    {
        for (int j = 0; j < shParam.dimNum(); ++j)
        {
            x[i] += feature[j] * shParam.pc(j, i);
        }
        x[i] -= shParam.minValue(i);
    }
    vector<char> bits(shParam.bitNum() / 8, 0);
    for (int i = 0; i < shParam.bitNum(); ++i)
    {
        double yi = 1.0;
        for (int j = 0; j < shParam.bitNum(); ++j)
        {
            yi *= sin(x[j] * shParam.omega(i, j) + PI / 2);
        }
        int a = i >> 3;
        int b = i & ((1 << 3) - 1);
        if (yi > 0)
        {
            bits[i >> 3] |= 1 << (i & ((1 << 3) - 1));
        }
    }
    return bits;
}

vector<vector<char>> Hashing::hashing(const Sketch& sketch)
{
    Config* config = Config::instance();
    int patchLength = config->patchLength;
    int shiftLength = config->shiftLength;
    int gridNum = config->gridNum;
    int binNum = config->binNum;
    int height = sketch.row();
    int width = sketch.col();
    double gridLength = (double)patchLength / gridNum;
    Filter filter;
    vector<vector<double>> gx = filter.filterSobelHorizon(sketch);
    vector<vector<double>> gy = filter.filterSobelVertical(sketch);
    vector<vector<double>> amplitude(height, vector<double>(width));
    vector<vector<int>> angle(height, vector<int>(width));
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            amplitude[i][j] = sqrt(gx[i][j] * gx[i][j] + gy[i][j] * gy[i][j]);
            angle[i][j] = (int)floor((atan2(gy[i][j], gx[i][j]) + 4 * PI) / (PI / binNum)) % binNum;
        }
    }
    int featureLen = gridNum * gridNum * binNum;
    vector<vector<char>> bits;
    for (int r = 0; r + patchLength <= height; r += shiftLength)
    {
        for (int c = 0; c + patchLength <= width; c += shiftLength)
        {
            vector<double> feature(featureLen, 0.0);
            for (int i = 0; i < patchLength; ++i)
            {
                for (int j = 0; j < patchLength; ++j)
                {
                    int row = (int)(i / gridLength);
                    int col = (int)(j / gridLength);
                    int index = (row * gridNum + col) * binNum + angle[r + i][c + j];
                    feature[index] += amplitude[r + i][c + j];
                }
            }
            double maxValue = 0.0;
            double minValue = 1e100;
            for (int i = 0; i < featureLen; ++i)
            {
                maxValue = max(maxValue, feature[i]);
                minValue = min(minValue, feature[i]);
            }
            for (int i = 0; i < featureLen; ++i)
            {
                feature[i] = (feature[i] - minValue) / (maxValue - minValue + 1e-6);
            }
            bits.push_back(hashing(feature));
        }
    }
    return bits;
}

vector<vector<char>> Hashing::read(const char* fileName)
{
    Config* config = Config::instance();
    int byteNum = config->shParam.bitNum() / 8;
    int index = 0;
    vector<vector<char>> bits;
    ifstream fin;
    fin.open(fileName, ios::in | ios::binary);
    while (!fin.eof())
    {
        bits.push_back(vector<char>(byteNum));
        for (int i = 0; i < byteNum; ++i)
        {
            fin >> bits[index][i];
        }
        ++index;
    }
    fin.close();
    return bits;
}

void Hashing::write(const char* fileName, vector<vector<char>>& bits)
{
    ofstream fout;
    fout.open(fileName, ios::out | ios::binary);
    for (auto bit : bits)
    {
        for (auto byte : bit)
        {
            fout << byte;
        }
    }
    fout.close();
}
