#pragma once

#include <opencv2/opencv.hpp>
#include "Matrix.h"

template<typename T = bool>
class Sketch : public Matrix<T>
{
public:
    Sketch() : Matrix()
    {
    }

    Sketch(const char* fileName) : Matrix()
    {
        read(fileName);
    }

    virtual ~Sketch()
    {
    }

    void read(const char* fileName)
    {
        IplImage* image = cvLoadImage(fileName, CV_LOAD_IMAGE_GRAYSCALE);
        initFromImage(image);
        cvReleaseImage(&image);
    }

    void initFromImage(IplImage* image)
    {
        init(image->height, image->width);
        for (int y = 0; y < image->height; ++y)
        {
            for (int x = 0; x < image->width; ++x)
            {
                _data[y][x] = ((unsigned char)((image->imageData + y * image->widthStep)[x])) < 128;
            }
        }
    }

    void write(const char* fileName)
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

    void inverse()
    {
        for (int y = 0; y < row(); ++y)
        {
            for (int x = 0; x < col(); ++x)
            {
                _data[y][x] = !_data[y][x];
            }
        }
    }
};
