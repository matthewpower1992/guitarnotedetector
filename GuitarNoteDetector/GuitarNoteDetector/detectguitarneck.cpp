#include "detectGuitarNeck.h"

void detectGuitarNeck(Mat src)
{
	Mat dst, color_dst, color_dst2;
	canny(src, dst, color_dst, color_dst2);

	vector<Vec4i> lines, lines2;
	vector<double> angles;

	Point p_max, p_min;
	p_min.x = src.cols;
	p_max.x = 0;
	p_min.y = src.rows;
	p_max.y = 0;

	HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 30, 10);
	lines2.resize(lines.size());
	angles.resize(lines.size());
	for (size_t i = 0; i < lines.size(); i++)
	{
		angles[i] = calculateAngle(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
	}
	sort(angles.begin(), angles.end());

	int numLines = 0;
	int pxmin_line, pxmax_line, pymin_line, pymax_line;

	double medianAngle = angles[angles.size() / 2];
	for (int i = 0; i < lines.size(); i++)
	{
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
		double deltaY = lines[i][3] - lines[i][1];
		double deltaX = lines[i][2] - lines[i][0];
		double angle = atan(deltaY / deltaX) * 180 / CV_PI;
		double mag = sqrt(deltaY*deltaY + deltaX*deltaX);
		if (!(angle > -2) && !(angle < -12) && mag > 100)
		{
			line(color_dst2, Point(lines[i][0], lines[i][1]),
				Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
			lines2[numLines] = lines[i];
			numLines++;
			int px1 = lines[i][0];
			int px2 = lines[i][2];
			int py1 = lines[i][1];
			int py2 = lines[i][3];

			if (lines[i][0] < p_min.x)
			{
				p_min.x = lines[i][0];
				p_min.y = lines[i][1];
				pxmin_line = i;
			}
			if (lines[i][1] > p_max.y)
			{
				p_max.x = lines[i][0];
				p_max.y = lines[i][1];
				pymax_line = i;
			}
			if (lines[i][2] > p_max.x)
			{
				p_max.x = lines[i][2];
				p_max.y = lines[i][3];
				pxmax_line = i;
			}
			if (lines[i][3] < p_min.y)
			{
				p_min.x = lines[i][2];
				p_min.y = lines[i][3];
				pymin_line = i;
			}
		}
	}
	double pxminAngle = calculateAngle(lines[pxmin_line][0], lines[pxmin_line][1], lines[pxmin_line][2], lines[pxmin_line][3]);
	double pxmaxAngle = calculateAngle(lines[pxmax_line][0], lines[pxmax_line][1], lines[pxmax_line][2], lines[pxmax_line][3]);
	double pyminAngle = calculateAngle(lines[pymin_line][0], lines[pymin_line][1], lines[pymin_line][2], lines[pymin_line][3]);
	double pymaxAngle = calculateAngle(lines[pymax_line][0], lines[pymax_line][1], lines[pymax_line][2], lines[pymax_line][3]);
	//rotate image based on angles
	circle(src, Point(p_max.x, p_max.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_min.x, p_min.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_max.x, p_max.y), 10, Scalar(255, 0, 0));
	circle(src, Point(p_min.x, p_min.y), 10, Scalar(255, 0, 0));



	//rectangle(src, Point(py_max.dx, py_max.dy), Point(py_min.dx, py_min.dy), Scalar(0, 0, 255));
	//determine roi from lines in color_dst2

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

void canny(Mat src, Mat &dst, Mat &color_dst, Mat &color_dst2)
{
	Canny(src, dst, 50, 100, 3);
	imwrite("Canny.jpg", dst);
	cvtColor(dst, color_dst, CV_GRAY2BGR);
	cvtColor(dst, color_dst2, CV_GRAY2BGR);
}