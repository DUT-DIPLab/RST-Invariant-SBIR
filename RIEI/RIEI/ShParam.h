#pragma once

#include <vector>
using namespace std;

class ShParam
{
public:
    ShParam();
    ~ShParam();

    void readParam(const char* filePath);

    int dimNum() const;
    int bitNum() const;
    
    double pc(int x, int y) const;
    double minValue(int index) const;
    double maxValue(int index) const;
    int mode(int x, int y) const;

    double omega(int x, int y) const;

    int hammingDist(vector<char>& a, vector<char>& b);

private:
    int _dimNum;
    int _bitNum;
    vector<vector<double>> _pc;
    vector<double> _minValue;
    vector<double> _maxValue;
    vector<vector<int>> _mode;

    vector<vector<double>> _omega;

    int _hammingDist[256];
};

inline int ShParam::dimNum() const
{
    return _dimNum;
}

inline int ShParam::bitNum() const
{
    return _bitNum;
}

inline double ShParam::pc(int x, int y) const
{
    return _pc[x][y];
}

inline double ShParam::minValue(int index) const
{
    return _minValue[index];
}

inline double ShParam::maxValue(int index) const
{
    return _maxValue[index];
}

inline int ShParam::mode(int x, int y) const
{
    return _mode[x][y];
}

inline double ShParam::omega(int x, int y) const
{
    return _omega[x][y];
}

inline int ShParam::hammingDist(vector<char>& a, vector<char>& b)
{
    int len = (int)a.size();
    int dist = 0;
    for (int i = 0; i < len; ++i)
    {
        dist += _hammingDist[(unsigned char)(a[i] ^ b[i])];
    }
    return dist;
}