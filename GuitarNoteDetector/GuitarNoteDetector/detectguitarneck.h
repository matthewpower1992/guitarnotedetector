#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

using namespace cv;

Mat detectGuitarNeck(Mat src, Mat orig, int &neckWidth, Mat &rot_mat, Rect &neckROI);

double calculateAngle(int px1, int py1, int px2, int py2);

double calculateMagnitude(int px1, int py1, int px2, int py2);

void maxOrMinPoint(int px1, int py1, int px2, int py2, int lineNum);

void rotatePoints(Mat rot_mat, Point &px, Point &py);