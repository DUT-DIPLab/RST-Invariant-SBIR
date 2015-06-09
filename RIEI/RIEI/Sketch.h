#pragma once

#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;

class Sketch
{
public:
    Sketch(int row = 0, int col = 0);
    Sketch(const char* fileName);

    void read(const char* fileName);
    void write(const char* fileName);
    void initFromImage(IplImage* image);

    int row() const;
    int col() const;
    double safeGet(int row, int col) const;
    vector<double>& operator[](int index);
    const vector<double>& operator[](int index) const;

    int getPositiveNum() const;
    double bilinearInterpolate(double r, double c) const; 

private:
    int _row, _col;
    vector<vector<double>> _data;
};

inline int Sketch::row() const
{
    return _row;
}

inline int Sketch::col() const
{
    return _col;
}

inline vector<double>& Sketch::operator[](int index)
{
    return _data[index];
}

inline const vector<double>& Sketch::operator[](int index) const
{
    return _data[index];
}