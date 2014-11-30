#include <opencv2/opencv.hpp>
#include "Config.h"
#include "Sketch.h"
#include "Preprocesser.h"

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

}