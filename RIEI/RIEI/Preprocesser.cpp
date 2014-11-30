#include <algorithm>
#include <opencv2/opencv.hpp>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"
using namespace std;

const int STEP_X[] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int STEP_Y[] = {0, 1, 1, 1, 0, -1, -1, -1};

Preprocesser::Preprocesser()
{
}

Preprocesser::~Preprocesser()
{
}

void Preprocesser::generateSketch(const char* input, const char* output)
{
    Config* config = Config::instance();
    IplImage* image = cvLoadImage(input, CV_LOAD_IMAGE_GRAYSCALE);
    Sketch<> edge;
    while (true)
    {
        IplImage* canny = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        cvCanny(image, canny, config->cannyLow, config->cannyHigh);
        edge.initFromImage(canny);
        edge.inverse();
        if (edge.getPositiveNum() < config->downSampleThres)
        {
            cvReleaseImage(&image);
            cvReleaseImage(&canny);
            break;
        }
        cvReleaseImage(&canny);
        int newWidth = (int)(image->width * config->downSampleRatio);
        int newHeight = (int)(image->height * config->downSampleRatio);
        IplImage* resize = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
        cvResize(image, resize, CV_INTER_CUBIC);
        cvReleaseImage(&image);
        image = resize;
    }
    edge.write(output);
}

void Preprocesser::cutOutSketch(const char* input, const char* output)
{
    Config* config = Config::instance();
    IplImage* image = cvLoadImage(input, CV_LOAD_IMAGE_GRAYSCALE);
    int length = min(image->width, image->height);
    int shift = abs(image->width - image->height) >> 1;
    if (image->width > image->height)
    {
        cvSetImageROI(image, cvRect(shift, 0, length, length));
    }
    else
    {
        cvSetImageROI(image, cvRect(0, shift, length, length));
    }
    IplImage* cropped = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
    cvCopy(image, cropped);
    cvReleaseImage(&image);
    IplImage* resized = cvCreateImage(cvSize(config->sketchWidth, config->sketchHeight), IPL_DEPTH_8U, 1);
    cvResize(cropped, resized, CV_INTER_CUBIC);
    cvReleaseImage(&cropped);
    Sketch<> sketch;
    sketch.initFromImage(resized);
    cvReleaseImage(&resized);
    sketch.write(output);
}

void Preprocesser::sketchThinning(const char* input, const char* output)
{
    Sketch<> sketch(input);
    for (int i = 0; i < sketch.row(); ++i)
    {
        sketch[i][0] = sketch[i][sketch.col() - 1] = false;
    }
    for (int i = 0; i < sketch.col(); ++i)
    {
        sketch[0][i] = sketch[sketch.row() - 1][i] = false;
    }
    bool deleted = true;
    bool oddIter = true;
    while (deleted)
    {
        deleted = false;
        for (int i = 1; i < sketch.row() - 1; ++i)
        {
            for (int j = 1; j < sketch.col() - 1; ++j)
            {
                if (sketch[i][j])
                {
                    bool p[10];
                    int neighborNum = 0;
                    for (int k = 0; k < 8; ++k)
                    {
                        int r = i + STEP_X[k];
                        int c = j + STEP_Y[k];
                        p[k + 2] = sketch[r][c];
                        neighborNum += sketch[r][c];
                    }
                    p[1] = p[9];
                    if (2 <= neighborNum && neighborNum <= 6)
                    {
                        int patternNum = 0;
                        for (int k = 1; k < 9; ++k)
                        {
                            patternNum += !p[k] && p[k + 1];
                        }
                        if (patternNum == 1)
                        {
                            if (oddIter)
                            {
                                if (!(p[2] && p[4] && p[6]) && !(p[4] && p[6] && p[8]))
                                {
                                    sketch[i][j] = false;
                                    deleted = true;
                                }
                            }
                            else
                            {
                                if (!(p[2] && p[4] && p[8]) && !(p[2] && p[6] && p[8]))
                                {
                                    sketch[i][j] = false;
                                    deleted = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        oddIter = !oddIter;
    }
    sketch.write(output);
}