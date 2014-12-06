#include <cmath>
#include <fstream>
#include "ShParam.h"

const double PI = acos(-1.0);

ShParam::ShParam()
{
    for (int i = 0; i < 256; ++i)
    {
        _hammingDist[i] = 0;
        for (int j = 0; j < 8; ++j)
        {
            if (i & (1 << j))
            {
                ++_hammingDist[i];
            }
        }
    }
}

ShParam::~ShParam()
{
}

void ShParam::readParam(const char* filePath)
{
    ifstream fin;
    fin.open(filePath, ios::in);
    fin >> _dimNum >> _bitNum;
    _pc = vector<vector<double>>(_dimNum, vector<double>(_bitNum));
    for (int i = 0; i < _dimNum; ++i)
    {
        for (int j = 0; j < _bitNum; ++j)
        {
            fin >> _pc[i][j];
        }
    }
    _minValue = vector<double>(_bitNum);
    for (int i = 0; i < _bitNum; ++i)
    {
        fin >> _minValue[i];
    }
    _maxValue = vector<double>(_bitNum);
    for (int i = 0; i < _bitNum; ++i)
    {
        fin >> _maxValue[i];
    }
    _mode = vector<vector<int>>(_bitNum, vector<int>(_bitNum));
    for (int i = 0; i < _bitNum; ++i)
    {
        for (int j = 0; j < _bitNum; ++j)
        {
            fin >> _mode[i][j];
        }
    }
    fin.close();
    vector<double> omega0(_bitNum);
    for (int i = 0; i < _bitNum; ++i)
    {
        omega0[i] = PI / (_maxValue[i] - _minValue[i]);
    }
    _omega = vector<vector<double>>(_bitNum, vector<double>(_bitNum));
    for (int i = 0; i < _bitNum; ++i)
    {
        for (int j = 0; j < _bitNum; ++j)
        {
            _omega[i][j] = _mode[i][j] * omega0[j];
        }
    }
}