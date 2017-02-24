#include "detectGuitarNeck.h"

int pxmin_line, pxmax_line, pymin_line, pymax_line;
Point p_max, p_min;

void detectGuitarNeck(Mat src)
{
	Mat dst, color_dst, color_dst2;
	canny(src, dst, color_dst, color_dst2);

	vector<Vec4i> lines, neckLines;
	vector<double> angles, sortedAngles;

	p_min.x = src.cols;
	p_max.x = 0;
	p_min.y = src.rows;
	p_max.y = 0;

	HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 30, 10);

	neckLines.resize(lines.size());
	angles.resize(lines.size());

	double angleSum = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		angles[i] = calculateAngle(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
		angleSum += angles[i];
	}

	int numNeckLines = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
		double mag = calculateMagnitude(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
		if (!(angles[i] > -2) && !(angles[i] < -12) && mag > 100)
		{
			line(color_dst2, Point(lines[i][0], lines[i][1]),
				Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
			neckLines[numNeckLines++] = lines[i];
			maxOrMinPoint(lines[i][0], lines[i][1], lines[i][2], lines[i][3], i);
		}
	}

	double pxminAngle = calculateAngle(lines[pxmin_line][0], lines[pxmin_line][1], lines[pxmin_line][2], lines[pxmin_line][3]);
	double pxmaxAngle = calculateAngle(lines[pxmax_line][0], lines[pxmax_line][1], lines[pxmax_line][2], lines[pxmax_line][3]);
	double pyminAngle = calculateAngle(lines[pymin_line][0], lines[pymin_line][1], lines[pymin_line][2], lines[pymin_line][3]);
	double pymaxAngle = calculateAngle(lines[pymax_line][0], lines[pymax_line][1], lines[pymax_line][2], lines[pymax_line][3]);

	double avgAngle = (pxminAngle + pxmaxAngle + pyminAngle + pymaxAngle) / 4;
	Mat rotatedImage = rotateImage(src, avgAngle);

	//rotate image based on angles
	circle(src, Point(p_max.x, p_max.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_min.x, p_min.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_max.x, p_max.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_min.x, p_min.y), 10, Scalar(255, 0, 0));

	namedWindow("Source", 1);
	imshow("Source", src);

	namedWindow("Detected Lines", 1);
	imshow("Detected Lines", color_dst2);

	namedWindow("Detected Lines2", 1);
	imshow("Detected Lines2", dst);
	imwrite("DetectedLines.jpg", color_dst2);

	waitKey(0);
}

double calculateAngle(int px1, int py1, int px2, int py2)
{
	double deltaY = py2 - py1;
	double deltaX = px2 - px1;
	double angle = atan((deltaY) / (deltaX)) * 180 / CV_PI;
	return angle;
}

double calculateMagnitude(int px1, int py1, int px2, int py2)
{
	double deltaY = py2 - py1;
	double deltaX = px2 - px1;
	double mag = sqrt(deltaY*deltaY + deltaX*deltaX);
	return mag;
}

void canny(Mat src, Mat &dst, Mat &color_dst, Mat &color_dst2)
{
	Canny(src, dst, 50, 100, 3);
	imwrite("Canny.jpg", dst);
	cvtColor(dst, color_dst, CV_GRAY2BGR);
	cvtColor(dst, color_dst2, CV_GRAY2BGR);
}

void maxOrMinPoint(int px1, int py1, int px2, int py2, int lineNum)
{
	if (px1 < p_min.x)
	{
		p_min.x = px1;
		p_min.y = py1;
		pxmin_line = lineNum;
	}
	if (py1 > p_max.y)
	{
		p_max.x = px1;
		p_max.y = py1;
		pymax_line = lineNum;
	}
	if (px2 > p_max.x)
	{
		p_max.x = px2;
		p_max.y = py2;
		pxmax_line = lineNum;
	}
	if (py2 < p_min.y)
	{
		p_min.x = px2;
		p_min.y = py2;
		pymin_line = lineNum;
	}
}

Mat rotateImage(Mat src, double angle)
{
	char* source_window = "Source image";
	char* rotate_window = "Warp + Rotate";

	Mat rot_mat(2, 3, CV_32FC1);
	Mat rotate_dst;

	/** Rotating the image after Warp */

	/// Compute a rotation matrix with respect to the center of the image
	Point center = Point(src.cols / 2, src.rows / 2);
	//double angle = -90.0;
	double scale = 1;

	/// Get the rotation matrix with the specifications above
	rot_mat = getRotationMatrix2D(center, angle, scale);

	/// Rotate the warped image
	warpAffine(src, rotate_dst, rot_mat, src.size());

	/// Show what you got
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, src);

	namedWindow(rotate_window, CV_WINDOW_AUTOSIZE);
	imshow(rotate_window, rotate_dst);

	/// Wait until user exits the program
	waitKey(0);

	return rotate_dst;
}