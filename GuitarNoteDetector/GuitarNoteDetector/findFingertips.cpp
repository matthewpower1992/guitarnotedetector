#include "findFingertips.h"

Mat segmented_image;
vector<Point> fingertips;
int numFingertips = 0;
int thresh = 210;
int max_thresh = 255;
RNG rng(12345);

vector<Point> findFingertips(Mat src, int &num)
{
	//Get pixels within skin colour range
	cvtColor(src, segmented_image, CV_BGR2GRAY);
	blur(segmented_image, segmented_image, Size(3, 3));
	cv::inRange(src, cv::Scalar(30, 60, 130), cv::Scalar(180, 180, 255), segmented_image);

	//Detect contours in image
	contourDetection();

	//Return fingertip locations and number detected in this iteration
	num = numFingertips;
	return fingertips;
}

void contourDetection()
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	//Perform canny edge detection
	Canny(segmented_image, canny_output, thresh, thresh * 2, 3);

	//Find contours in image
	findContours(segmented_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	fingertips.resize(contours.size());

	int maxY = segmented_image.rows;
	bool drawn = false;

	//If contour is between 10 and 60 pixels and within the right half of the image, likely a fingertip
	for (int i = 0; i< contours.size(); i++, drawn = false)
	{
		if (contours[i].capacity() > 10 && contours[i].capacity() < 60 
			&& contours[i][0].x > 0.60*segmented_image.cols && contours[i][0].x < 0.95*segmented_image.cols) 
		{
			int finX = segmented_image.cols;
			int finY = segmented_image.rows;
			for (int j = 0; drawn == false && j < contours[i].capacity(); j++)
			{
				if (contours[i][j].x < finX && contours[i][j].y < finY) //find leftmost pixel as fingertip maxima
				{
					finX = contours[i][j].x;
					finY = contours[i][j].y;
				}
				if (contours[i][j].y > maxY - 5)
				{
					drawn = true;
				}
			}
			if (drawn)
			{
				Point fingertipCentre = Point(finX, finY);
				fingertips[numFingertips++] = fingertipCentre;
			}
		}
	}
}