#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

using namespace cv;

Mat detectGuitarNeck(Mat src, Mat orig);

double calculateAngle(int px1, int py1, int px2, int py2);

double calculateMagnitude(int px1, int py1, int px2, int py2);

void canny(Mat src, Mat &dst, Mat &color_dst, Mat &color_dst2);

void maxOrMinPoint(int px1, int py1, int px2, int py2, int lineNum);

Mat rotateImage(Mat &src, Mat rot_mat);

void rotatePoints(Mat rot_mat, Point &px, Point &py);