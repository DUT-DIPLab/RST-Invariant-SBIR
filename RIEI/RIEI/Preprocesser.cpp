#include <algorithm>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"

const int STEP_X[] = {-1, -1, 0, 1, 1, 1, 0, -1};
const int STEP_Y[] = {0, 1, 1, 1, 0, -1, -1, -1};

Preprocesser::Preprocesser()
{
}

Preprocesser::~Preprocesser()
{
}

Sketch Preprocesser::preprocess(const char* filePath)
{
    return cutOutSketch(generateEdges(filePath));
}

Sketch Preprocesser::generateEdges(const char* filePath)
{
    Config* config = Config::instance();
    IplImage* image = cvLoadImage(filePath, CV_LOAD_IMAGE_GRAYSCALE);
    Sketch edge;
    IplImage* canny = nullptr;
    int height = image->height;
    int width = image->width;
    double ratio = 1.0;
    while (true)
    {
        canny = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        cvCanny(image, canny, config->cannyLow, config->cannyHigh);
        edge.initFromImage(canny);
        if (edge.getPositiveNum() < config->downSampleThres)
        {
            cvReleaseImage(&image);
            cvReleaseImage(&canny);
            break;
        }
        cvReleaseImage(&canny);
        ratio *= config->downSampleRatio;
        int newWidth = (int)(image->width * ratio);
        int newHeight = (int)(image->height * ratio);
        IplImage* resize = cvCreateImage(cvSize(newWidth, newHeight), IPL_DEPTH_8U, 1);
        cvResize(image, resize, CV_INTER_CUBIC);
        cvResize(resize, image, CV_INTER_CUBIC);
        cvReleaseImage(&resize);
    }
    return edge;
}

Sketch Preprocesser::cutOutSketch(const Sketch&& edge)
{
    Config* config = Config::instance();
    int totalNum = 0;
    double cr = 0.0;
    double cc = 0.0;
    for (int r = 0; r < edge.row(); ++r)
    {
        for (int c = 0; c < edge.col(); ++c)
        {
            if (edge[r][c])
            {
                cr += r;
                cc += c;
                ++totalNum;
            }
        }
    }
    cr /= totalNum;
    cc /= totalNum;
    double dmean = 0.0;
    for (int r = 0; r < edge.row(); ++r)
    {
        for (int c = 0; c < edge.col(); ++c)
        {
            if (edge[r][c])
            {
                double x = r - cr;
                double y = c - cc;
                dmean += sqrt(x * x + y * y);
            }
        }
    }
    dmean /= totalNum;
    double radius = dmean * 1.85;
    double diameter = radius * 2.0;
    double sr = cr - radius;
    double sc = cc - radius;
    Sketch sketch(config->sketchSideLength, config->sketchSideLength);
    for (int r = 0; r < sketch.row(); ++r)
    {
        for (int c = 0; c < sketch.col(); ++c)
        {
            double er = sr + (double)r / sketch.row() * diameter;
            double ec = sc + (double)c / sketch.col() * diameter;
            sketch[r][c] = edge.bilinearInterpolate(er, ec);
        }
    }
    return sketch;
}
