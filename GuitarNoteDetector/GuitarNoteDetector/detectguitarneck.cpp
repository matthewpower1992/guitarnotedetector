#include "detectGuitarNeck.h"

int pxmin_line, pxmax_line, pymin_line, pymax_line;
Point p_max, p_min;

Mat detectGuitarNeck(Mat src, Mat orig)
{
	//color dst used for debug
	Mat dst, color_dst, color_dst2;
	Mat src_gray;
	blur(src, src_gray, Size(4, 4));
	Canny(src_gray, dst, 100, 150, 3);
	imwrite("Canny.jpg", dst);
	cvtColor(dst, color_dst, CV_GRAY2BGR);
	cvtColor(dst, color_dst2, CV_GRAY2BGR);

	//necklines, centrepoints/rotatedcentrepoints only used for debug
	vector<Vec4i> lines, neckLines;
	vector<Point> centrePoints, rotatedCentrePoints;
	vector<double> angles;

	p_min.x = src.cols;
	p_max.x = 0;
	p_min.y = src.rows;
	p_max.y = 0;

	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 75, 10);

	neckLines.resize(lines.size());
	centrePoints.resize(lines.size());
	angles.resize(lines.size());

	double angleSum = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		angles[i] = calculateAngle(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
	}

	int numNeckLines = 0;

	for (int i = 0; i < lines.size(); i++)
	{
		//coloring line used for debug
		line(color_dst, Point(lines[i][0], lines[i][1]),
			Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
		double mag = calculateMagnitude(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
		int centreX = abs(lines[i][2] + lines[i][0]) / 2;
		if (angles[i] < 0  && angles[i] > -30 && mag > 75 && centreX < 0.80*src.cols)
		{
			//coloring line used for debug
			line(color_dst2, Point(lines[i][0], lines[i][1]),
				Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, 8);
			
			neckLines[numNeckLines] = lines[i];
			angleSum += angles[i];
			centrePoints[numNeckLines++] = Point(abs(lines[i][2]+lines[i][0])/2, 
				abs(lines[i][3] + lines[i][1])/2);			
			maxOrMinPoint(lines[i][0], lines[i][1], lines[i][2], lines[i][3], i);
		}
	}

	//namedWindow("cd", 1);
	//imshow("cd", color_dst);
	imwrite("cd.jpg", color_dst);
	//namedWindow("cd2", 1);
	//imshow("cd2", color_dst2);
	imwrite("cd2.jpg", color_dst2);
	double avgAngle = angleSum / numNeckLines;

	Point center = Point(orig.cols / 2, orig.rows / 2);
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center, avgAngle, 1);

	Mat rotatedImage;
	warpAffine(orig, rotatedImage, rot_mat, orig.size());

	transform(centrePoints, rotatedCentrePoints, rot_mat);
	int maxY = 0;
	int minY = orig.rows;
	for (int i = 0; i < numNeckLines; i++)
	{
		int y = rotatedCentrePoints[i].y;
		if (y > maxY)
		{
			maxY = y;
		}
		if (y < minY)
		{
			minY = y;
		}
		//circle(src, centrePoints[i], 10, Scalar(255, 255, 0));
		//circle(rotatedImage, rotatedCentrePoints[i], 10, Scalar(255, 255, 0));
		
	}
	int neckWidth = maxY - minY;
	Point px = Point(p_min.x, p_max.y);
	Point py = Point(p_max.x, p_min.y);
	rotatePoints(rot_mat, px, py);
	int rotNeckWidth = px.y-py.y;
	int neckError = (rotNeckWidth - neckWidth) / 2;
	circle(rotatedImage, px, 2, Scalar(0, 255, 0));
	circle(rotatedImage, py, 2, Scalar(0, 255, 0));
	Point px1 = Point(px.x, px.y - neckError);
	Point py1 = Point(py.x, py.y + neckError);
	circle(rotatedImage, px1, 2, Scalar(0, 255, 255));
	circle(rotatedImage, py1, 2, Scalar(0, 255, 255));

	namedWindow("rot", 1);
	imshow("rot", rotatedImage);
	namedWindow("src", 1);
	imshow("src", src);
	//waitKey(0);
	
	Rect neckROI(px1.x, py1.y, py1.x-px1.x, px1.y-py1.y); //getting neg values 
	Mat cropped;
	rotatedImage(neckROI).copyTo(cropped);

	imwrite("neckk.jpg", cropped);
	//waitKey(0);
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