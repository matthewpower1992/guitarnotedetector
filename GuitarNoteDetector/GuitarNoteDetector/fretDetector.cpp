#include "fretDetector.h"

vector<int> fretDetector(Mat src)
{
	Mat src_gray, cdst;
	Mat grad;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	//Blur neck image
	GaussianBlur(src, src, Size(3, 3), 0, 0, BORDER_DEFAULT);
	cvtColor(src, src_gray, CV_BGR2GRAY);

	//Get sobel x image
	Mat grad_x, abs_grad_x;
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	vector<int> fretEnds;

	//Threshold sobel image
	Mat neckbw;
	threshold(abs_grad_x, neckbw, 75, 255, THRESH_BINARY);
	int yval = abs_grad_x.rows;

	//Get yvalue approximately 70% of the height of the image
	yval = yval / 16;
	yval = yval * 5;

	fretEnds.resize(20);
	int numPoints = 0;
	int prevColor = 255;
	int fretWidth = 0;
	int fretStart = 0;
	int fretEnd = 0;

	//Find fret markers by tracing across neck and finding large stretches of black pixels
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
					fretEnds[numPoints++] = fretEnd;

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

	return fretEnds;
}