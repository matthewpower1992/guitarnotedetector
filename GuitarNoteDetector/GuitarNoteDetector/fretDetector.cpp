#include "fretDetector.h"

vector<Point> fretDetector(Mat src)
{
	Mat src_gray, cdst;
	Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(src, src_gray, CV_BGR2GRAY);

	Mat grad_x, abs_grad_x;
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	vector<Point> frets;
	vector<double> angles, sortedAngles;

	Mat neckbw;
	threshold(abs_grad_x, neckbw, 75, 255, THRESH_BINARY);
	int yval = abs_grad_x.rows;
	yval = yval / 16;
	yval = yval * 5;
	//used for debug
	imwrite("gard_neck.png", neckbw);

	frets.resize(abs_grad_x.cols);
	int numPoints = 0;
	int prevColor = 255;
	int fretWidth = 0;
	int fretStart = 0;
	int fretEnd = 0;
	for (int i = abs_grad_x.cols - 1; i >= 0; i--)
	{
		int color = (int)neckbw.at<uchar>(Point(i, yval));
		if (color == 255) 
		{
			if (prevColor == 0)
			{
				if (fretWidth > 10)
				{
					fretEnd = i;
					frets[numPoints++] = Point(fretEnd, yval);
				}
				fretWidth = 0;
			}
		}
		else
		{
			fretWidth++;			
		}
		prevColor = color;
	}
	//all below used for debug
	for (int i = 0; i < numPoints; i++)
	{
		circle(src, frets[i], 3, Scalar(0, 255, 0));
	}


	namedWindow("grad", 1);
	imshow("grad", abs_grad_x);
	namedWindow("nbw", 1);
	imshow("nbw", neckbw);
	namedWindow("src11", 1);
	imshow("src11", src);


	//waitKey(0);

	return frets;
}