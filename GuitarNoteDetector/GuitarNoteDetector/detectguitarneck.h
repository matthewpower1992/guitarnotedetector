#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

using namespace cv;

void detectGuitarNeck(Mat src);

double calculateAngle(int px1, int py1, int px2, int py2);

double calculateMagnitude(int px1, int py1, int px2, int py2);

void canny(Mat src, Mat &dst, Mat &color_dst, Mat &color_dst2);

void maxOrMinPoint(int px1, int py1, int px2, int py2, int lineNum);

Mat rotateImage(Mat src, double angle);