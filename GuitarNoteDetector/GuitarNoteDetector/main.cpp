#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

#include "detectGuitarNeck.h"

using namespace cv;

int main(int argc, char** argv)
{
	Mat src;
	src = imread("guitar1.png");
	Mat neck = detectGuitarNeck(src);

	namedWindow("Source", 1);
	imshow("Source", src);

	namedWindow("Neck", 1);
	imshow("Neck", neck);

	waitKey(0);


	return 0;
}