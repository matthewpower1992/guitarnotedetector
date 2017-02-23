#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

#include "detectGuitarNeck.h"

using namespace cv;

int main(int argc, char** argv)
{
	Mat src;
	src = imread("guitar1.png");
	detectGuitarNeck(src);

	return 0;
}