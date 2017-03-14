#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>
#include <sstream>

using namespace cv;


Mat background_sub(Mat background, Mat frame);

Mat dilation(Mat src);

//void processVideo(char* videoFilename);

