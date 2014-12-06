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

IplImage* Preprocesser::generateEdges(const char* filePath)
{
    Config* config = Config::instance();
    IplImage* image = cvLoadImage(filePath, CV_LOAD_IMAGE_GRAYSCALE);
    Sketch edge;
    IplImage* canny = nullptr;
    while (true)
    {
        canny = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        cvCanny(image, canny, config->cannyLow, config->cannyHigh);
        edge.initFromImage(canny);
        if (edge.getPositiveNum() < config->downSampleThres)
        {
            cvReleaseImage(&image);
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
    return canny;
}

Sketch Preprocesser::cutOutSketch(IplImage* canny)
{
    Config* config = Config::instance();
    int length = min(canny->width, canny->height);
    int shift = abs(canny->width - canny->height) >> 1;
    if (canny->width > canny->height)
    {
        cvSetImageROI(canny, cvRect(shift, 0, length, length));
    }
    else
    {
        cvSetImageROI(canny, cvRect(0, shift, length, length));
    }
    IplImage* cropped = cvCreateImage(cvGetSize(canny), IPL_DEPTH_8U, 1);
    cvCopy(canny, cropped);
    cvReleaseImage(&canny);
    IplImage* resized = cvCreateImage(cvSize(config->sketchSideLength, config->sketchSideLength), IPL_DEPTH_8U, 1);
    cvResize(cropped, resized, CV_INTER_CUBIC);
    cvReleaseImage(&cropped);
    Sketch sketch;
    sketch.initFromImage(resized);
    cvReleaseImage(&resized);
    return sketch;
}
