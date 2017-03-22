#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>

#include "detectGuitarNeck.h"
#include "findFingertips.h"
#include "fretDetector.h"

using namespace cv;

bool capture();

int main(int argc, char** argv)
{
	capture();
	waitKey(0);
	Mat mask, guitarist, maskedImage, guitarNeck;
	vector<Point> fingertips, frets;
	mask = imread("2_mask.png");
	guitarist = imread("2_35.png");
	guitarist.copyTo(maskedImage, mask);
	guitarNeck = detectGuitarNeck(maskedImage, guitarist);
	frets = fretDetector(guitarNeck);
	fingertips = findFingertips(guitarNeck);
	namedWindow("Mask", 1);
	moveWindow("Mask", 0, 0);
	imshow("Mask", mask);
	namedWindow("Guitarist", 1);
	moveWindow("Guitarist", 500, 0);
	imshow("Guitarist", guitarist);
	namedWindow("Masked Image", 1);
	moveWindow("Masked Image", 500, 500);
	imshow("Masked Image", maskedImage);
	namedWindow("Guitar Neck", 1);
	moveWindow("Guitar Neck", 700, 500);
	imshow("Guitar Neck", guitarNeck);
	waitKey(0);		

	//namedWindow("Source", 1);
	//imshow("Source", src);

	//namedWindow("Back", 1);
	//imshow("Back", bgs);

	//namedWindow("Neck", 1);
	//imshow("Neck", neck);

	//namedWindow("Fingertips", 1);
	//imshow("Fingertips", dest);

	//namedWindow("Frets", 1);
	//imshow("Frets", frets);

	waitKey(0);


	return 0;
}

bool capture()
{
	Mat img, mask, maskedImg;
	Ptr<BackgroundSubtractor> subtractor = new BackgroundSubtractorMOG2();
	VideoCapture capture(0);
	if (!capture.isOpened())  
		return false;
	namedWindow("Guitar Note Detector", 1);
	moveWindow("Guitar Note Detector", 0, 0);
	time_t start = time(0);
	int duration = 40;
	int maskCap = 20;
	for (int i = start; difftime(time(0), i) < duration;)
	{
		if (!capture.grab())
		{
			std::cout << "Video failed to capture" << std::endl;
		}
		capture.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
		if (difftime(time(0), start) < maskCap)
		{
			if (mask.empty()) {
				mask.create(img.size(), img.type());
			}
			subtractor->operator()(img, mask, -1);
			GaussianBlur(mask, mask, Size(11, 11), 3.5, 3.5);
			threshold(mask, mask, 10, 255, THRESH_BINARY);
		}
		//only need these for debug video
		maskedImg = Scalar::all(0);
		img.copyTo(maskedImg, mask);
		//need to delay
		waitKey(40);

		string text1 = "Please step out of frame";
		string text2 = "Calibrating";
		string text3 = "Step back into frame with guitar";
		string text4 = "Begin";
		int fontFace = FONT_HERSHEY_PLAIN;
		double fontScale = 2;
		int thickness = 3;
		cv::Point textOrg(100, 130);
		int backgroundCalib = 5;
		int backgroundCap = 10;
		int maskCap = 20;
		int noteRate = 3;
		int seconds = difftime(time(0), start);

		if (seconds < backgroundCalib)
		{
			cv::putText(img, text1, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (seconds >= backgroundCalib && seconds <= backgroundCap)
		{
			cv::putText(img, text2, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (seconds > backgroundCap && seconds < maskCap)
		{
			cv::putText(img, text3, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (seconds == maskCap)
		{			
			string filename = "2_";
			filename += std::to_string(seconds);
			string maskname = "2_mask.png";
			filename += ".png";
			imwrite(filename, img);
			imwrite(maskname, mask);
			cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

		}
		else if (seconds % noteRate == (noteRate - 1))
		{
			string filename = "2_";
			filename += std::to_string(seconds);
			filename += ".png";
			imwrite(filename, img);
		}
		imshow("Guitar Note Detector", img);
	}

}

/*Mat dilation(Mat src)
{
	Mat dilation_dst;
	//namedWindow("Dilation Demo", CV_WINDOW_AUTOSIZE);
	//cvMoveWindow("Dilation Demo", src.cols, 0);

	int dilation_elem = 1;
	int dilation_size = 2;
	int dilation_type = MORPH_ELLIPSE;

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	dilate(src, dilation_dst, element);
	//imshow("Dilation Demo", dilation_dst);
	return dilation_dst;
	}*/