#include "Sketch.h"

Sketch::Sketch(int row, int col) : _row(row), _col(col)
{
    if (row > 0 && col > 0)
    {
        _data = vector<vector<bool>>(row, vector<bool>(col, false));
    }
}

Sketch::Sketch(const char* fileName)
{
    read(fileName);
}

void Sketch::read(const char* fileName)
{
    IplImage* image = cvLoadImage(fileName, CV_LOAD_IMAGE_GRAYSCALE);
    initFromImage(image);
    cvReleaseImage(&image);
}

void Sketch::write(const char* fileName)
{
    IplImage* image = cvCreateImage(cvSize(col(), row()), IPL_DEPTH_8U, 1);
    for (int y = 0; y < image->height; ++y)
    {
        for (int x = 0; x < image->width; ++x)
        {
            (image->imageData + y * image->widthStep)[x] = _data[y][x] ? 0 : 255;
        }
    }
    cvSaveImage(fileName, image);
    cvReleaseImage(&image);
}

void Sketch::initFromImage(IplImage* image)
{
    _row = image->height;
    _col = image->width;
    _data = vector<vector<bool>>(_row, vector<bool>(_col, false));
    for (int y = 0; y < image->height; ++y)
    {
        for (int x = 0; x < image->width; ++x)
        {
            _data[y][x] = ((unsigned char)((image->imageData + y * image->widthStep)[x])) < 128;
        }
    }
}

void Sketch::inverse()
{
    for (int y = 0; y < row(); ++y)
    {
        for (int x = 0; x < col(); ++x)
        {
            _data[y][x] = !_data[y][x];
        }
    }
}

double Sketch::bilinearInterpolate(double r, double c) const
{
    int fr = (int)r;
    int fc = (int)c;
    int cr = fr + 1;
    int cc = fc + 1;
    double alpha = c - fc;
    double v1 = safeGet(fr, fc) + alpha * (safeGet(fr, cc) - safeGet(fr, fc));
    double v2 = safeGet(cr, fc) + alpha * (safeGet(cr, cc) - safeGet(cr, fc));
    double beta = r - fr;
    return v1 + beta * (v2 - v1);
}

double Sketch::safeGet(int row, int col) const
{
    if (0 <= row && row < _row)
    {
        if (0 <= col && col < _col)
        {
            return _data[row][col];
        }
    }
    return 0.0;
}

int Sketch::getPositiveNum() const
{
    int num = 0;
    for (int i = 0; i < row(); ++i)
    {
        for (int j = 0; j < col(); ++j)
        {
            if (_data[i][j])
            {
                ++num;
            }
        }
    }
    return num;
}

Sketch Sketch::crop(int top, int left, int height, int width) const
{
    Sketch cropped(height, width);
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            cropped[i][j] = _data[i + top][j + left];
        }
    }
    return cropped;
}