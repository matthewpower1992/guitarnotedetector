#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>

#include "detectGuitarNeck.h"
#include "background_sub.h"

using namespace cv;

int main(int argc, char** argv)
{
	Mat background, src, bgs, neck;
	background = imread("1.jpg");
	src = imread("2.jpg");
	bgs = background_sub(background, src);
	neck = detectGuitarNeck(bgs);

	namedWindow("Source", 1);
	imshow("Source", src);

	namedWindow("Back", 1);
	imshow("Back", bgs);

	//namedWindow("Neck", 1);
	//imshow("Neck", neck);

	waitKey(0);


	return 0;
}