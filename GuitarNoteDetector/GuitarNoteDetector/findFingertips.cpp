#include "findFingertips.h"

Mat segmented_image;
vector<Point> fingertips;
int numFingertips = 0;
int thresh = 210;
int max_thresh = 255;
RNG rng(12345);

Mat findFingertips(Mat src)
{
	cvtColor(src, segmented_image, CV_BGR2GRAY);
	blur(segmented_image, segmented_image, Size(3, 3));
	//cv::inRange(src, cv::Scalar(80, 40, 20), cv::Scalar(255, 255, 255), segmented_image);
	cv::inRange(src, cv::Scalar(80, 40, 20), cv::Scalar(255, 255, 255), segmented_image);

	/*char* source_window = "Source";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, src);*/

	thresh_callback(0, 0);

	for (int i = 0; i < numFingertips; i++)
	{
		circle(src, fingertips[i], 5, Scalar(0, 255, 0));
	}

	namedWindow("Segmented", 1);
	/*imwrite("seg.png", segmented_image);
	imshow("Segmented", segmented_image);

	namedWindow("Neck", 1);
	imshow("Neck", src);
	imwrite("circ.png", src);*/

	return segmented_image;
}







void thresh_callback(int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(segmented_image, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	fingertips.resize(contours.size());

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	Mat drawing2 = Mat::zeros(canny_output.size(), CV_8UC3);
	int maxY = segmented_image.rows;
	bool drawn = false;
	for (int i = 0; i< contours.size(); i++, drawn = false)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		if (contours[i].capacity() > 20 && contours[i].capacity() < 30) //if contour within right side of image? 
		{
			int finX = segmented_image.cols;
			int finY = segmented_image.rows;
			for (int j = 0; drawn == false && j < contours[i].capacity(); j++)
			{
				if (contours[i][j].x < finX && contours[i][j].y < finY) //find leftmost pixel (change to upper left)
				{
					finX = contours[i][j].x;
					finY = contours[i][j].y;
				}
				if (contours[i][j].y > maxY - 5)
				{
					drawContours(drawing2, contours, i, color, 2, 8, hierarchy, 0, Point());
					drawn = true;
				}
			}
			if (drawn)
			{
				Point fingertipCentre = Point(finX, finY);
				//circle(drawing2, fingertipCentre, 5, Scalar(0, 255, 0));
				fingertips[numFingertips++] = fingertipCentre;
			}
		}
	}

	/// Show in a window
	/*namedWindow("Drawing", CV_WINDOW_AUTOSIZE);
	imshow("Drawing", drawing);
	imwrite("contours.png", drawing);
	namedWindow("Drawing2", CV_WINDOW_AUTOSIZE);
	imshow("Drawing2", drawing2);
	imwrite("con2.png", drawing2);*/
}