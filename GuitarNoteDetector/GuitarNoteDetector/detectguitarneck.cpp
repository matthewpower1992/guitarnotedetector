#include "detectGuitarNeck.h"

int pxmin_line, pxmax_line, pymin_line, pymax_line;
Point p_max, p_min;

Mat detectGuitarNeck(Mat src)
{
	Mat dst, color_dst, color_dst2;
	canny(src, dst, color_dst, color_dst2);

	vector<Vec4i> lines, neckLines;
	vector<Point> centrePoints;
	vector<double> angles, sortedAngles;

	p_min.x = src.cols;
	p_max.x = 0;
	p_min.y = src.rows;
	p_max.y = 0;

	HoughLinesP(dst, lines, 1, CV_PI / 180, 80, 30, 10);

	neckLines.resize(lines.size());
	centrePoints.resize(lines.size());
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
		//if (!(angles[i] > -2) && !(angles[i] < -12) && mag > 100) //need to rework this
		if (angles[i] < 0  && angles[i] > -30 && mag > 75)
		{
			line(color_dst2, Point(lines[i][0], lines[i][1]),
				Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
			
			neckLines[numNeckLines] = lines[i];
			centrePoints[numNeckLines++] = Point(abs(lines[i][2]+lines[i][0])/2, 
				abs(lines[i][3] + lines[i][1])/2);			
			//circle(color_dst2, centrePoints[numNeckLines - 1], 5, Scalar(0, 255, 0));
			//line with highest centre point and lowest centre point, after removing outliers
			//rotate image based on avg angle, determine diff between y values?
			//probs dont need max/min point then
			maxOrMinPoint(lines[i][0], lines[i][1], lines[i][2], lines[i][3], i);
		}
	}

	//namedWindow("cd", 1);
	//imshow("cd", color_dst);
	imwrite("cd.jpg", color_dst);
	//namedWindow("cd2", 1);
	//imshow("cd2", color_dst2);
	imwrite("cd2.jpg", color_dst2);

	double pxminAngle = calculateAngle(lines[pxmin_line][0], lines[pxmin_line][1], lines[pxmin_line][2], lines[pxmin_line][3]);
	double pxmaxAngle = calculateAngle(lines[pxmax_line][0], lines[pxmax_line][1], lines[pxmax_line][2], lines[pxmax_line][3]);
	double pyminAngle = calculateAngle(lines[pymin_line][0], lines[pymin_line][1], lines[pymin_line][2], lines[pymin_line][3]);
	double pymaxAngle = calculateAngle(lines[pymax_line][0], lines[pymax_line][1], lines[pymax_line][2], lines[pymax_line][3]);

	double avgAngle = (pxminAngle + pxmaxAngle + pyminAngle + pymaxAngle) / 4;
	Point center = Point(src.cols / 2, src.rows / 2);
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center, avgAngle, 1);

	Mat rotatedImage;
	warpAffine(src, rotatedImage, rot_mat, src.size());

	Point px = Point(p_min.x, p_max.y);
	Point py = Point(p_max.x, p_min.y);
	rotatePoints(rot_mat, px, py);	
	
	Rect neckROI(px.x, py.y, py.x-px.x, px.y-py.y); //getting neg values 
	Mat cropped;
	rotatedImage(neckROI).copyTo(cropped);

	imwrite("neckk.jpg", cropped);
	return cropped;
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
		pxmin_line = lineNum;
	}
	if (py1 > p_max.y)
	{
		p_max.y = py1;
		pymax_line = lineNum;
	}
	if (px2 > p_max.x)
	{
		p_max.x = px2;
		pxmax_line = lineNum;
	}
	if (py2 < p_min.y)
	{
		p_min.y = py2;
		pymin_line = lineNum;
	}
}

void rotatePoints(Mat rot_mat, Point &px, Point &py)
{
	std::vector<Point> vec;
	vec.push_back(px);
	vec.push_back(py);
	transform(vec, vec, rot_mat);
	px = vec.at(0);
	px.y += 10;
	py = vec.at(1);
	py.y -= 10;
}