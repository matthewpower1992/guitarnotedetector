#include "fretDetector.h"

Mat fretDetector(Mat src)
{
	Mat src_gray, cdst;
	Mat grad;
	char* window_name = "Sobel Demo - Simple Edge Detector";
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	int c;

	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);

	/// Convert it to gray
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// Create window
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	imshow(window_name, grad);

	//////////////////////////////////////////////
	Mat color_dst;
	vector<Vec4i> lines, neckLines;
	vector<Point> centrePoints;
	vector<double> angles, sortedAngles;

	cvtColor(grad, color_dst, CV_GRAY2BGR);

	Mat neckbw;
	threshold(abs_grad_x, neckbw, 75, 255, THRESH_BINARY);
	int yval = abs_grad_x.rows;
	yval = yval / 16;
	yval = yval * 5;
	imwrite("gard_neck.png", neckbw);
	centrePoints.resize(abs_grad_x.cols);
	int numPoints = 0;
	//line(neckbw, Point(0, yval),
	//	Point(abs_grad_x.cols, yval), Scalar(0, 0, 255), 3, 8);
	bool fretStarted = false;
	int prevColor = 255;
	int fretWidth = 0;
	int fretStart = 0;
	int fretEnd = 0;
	for (int i = abs_grad_x.cols - 1; i >= 0; i--)
	{
		int color = (int)neckbw.at<uchar>(Point(i, yval));
		if (color == 255) //if pixel is white
		{
			if (prevColor == 0)
			{
				if (fretWidth > 10)
				{
					fretEnd = i;
					centrePoints[numPoints++] = Point(fretEnd, yval);
				}
				fretWidth = 0;
			}
			//if (fretStarted == false)
			//{
			//	fretStarted = true;
			//	centrePoints[numPoints++] = Point(i, yval);
			//}
		}
		else
		{
			//if (prevColor == 255)
			//{
			//	fretStart = i;
			//}
			fretWidth++;			
		}
		prevColor = color;
	}
	for (int i = 0; i < numPoints; i++)
	{
		circle(src, centrePoints[i], 3, Scalar(0, 255, 0));
	}


	namedWindow("grad", 1);
	imshow("grad", abs_grad_x);
	namedWindow("nbw", 1);
	imshow("nbw", neckbw);
	namedWindow("src11", 1);
	imshow("src11", src);


	//waitKey(0);

	return grad;
}