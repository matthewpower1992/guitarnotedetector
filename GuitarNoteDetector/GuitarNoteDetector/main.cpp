#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>

#include "detectGuitarNeck.h"
#include "background_sub.h"
#include "findFingertips.h"
#include "fretDetector.h"

using namespace cv;

bool record();
void backsub();

int main(int argc, char** argv)
{
	//PlaySound(TEXT("hello.wav"),NULL, SND_FILENAME);
	
	backsub();
	waitKey(0);
	Mat background, src, bgs, neck, fingerTips, frets,img;
	src = imread("2_w.png");
	background = imread("2_3.png");
	background.copyTo(img, src);
	namedWindow("img", 1);
	imshow("img", img);
	neck = detectGuitarNeck(img);
	waitKey(0);
	/*int dilation_elem = 1;
	int dilation_size = 5;
	int dilation_type = MORPH_ELLIPSE;

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	bgs = dilation(src);
	erode(bgs, neck, element);*/
	
	/*Mat dest;
	background.copyTo(dest);
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			int color = (int)bgs.at<uchar>(Point(j, i));
			if (color <= 127)
			{
				dest.at<uchar>(Point(j * 3, i)) = 127;
				dest.at<uchar>(Point((j * 3) + 1, i)) = 127;
				dest.at<uchar>(Point((j * 3) + 2, i)) = 127;
			}
		}
	}*/

	
	//backsub();
	//background = imread("1_w.png");
	//src = imread("2_w.png");
	//absdiff(background, src, bgs);
	//if (!record())
	//{
	//	return (-1);
	//}
	//bgs = background_sub(background, src);
	//neck = detectGuitarNeck(bgs);
	//fingerTips = findFingertips(neck);
	//frets = fretDetector(neck);
	//frets = fretDetector(src);
	//bgs = background_sub(background, src); //if mask too big use src?
	//neck = detectGuitarNeck(bgs);
	//fingerTips = findFingertips(neck);

	namedWindow("Source", 1);
	imshow("Source", src);

	namedWindow("Back", 1);
	imshow("Back", bgs);

	namedWindow("Neck", 1);
	imshow("Neck", neck);

	//namedWindow("Fingertips", 1);
	//imshow("Fingertips", dest);

	//namedWindow("Frets", 1);
	//imshow("Frets", frets);

	waitKey(0);


	return 0;
}

void backsub()
{
	// Init background substractor
	Ptr<BackgroundSubtractor> bg_model = new BackgroundSubtractorMOG2();

	// Create empy input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;

	// capture video from source 0, which is web camera, If you want capture video from file just replace //by  VideoCapture cap("videoFile.mov")
	VideoCapture cap(0);
	namedWindow("Guitar Note Detector", 1);
	time_t start = time(0);
	int secs;
	// main loop to grab sequence of input files
	for (int i = start; difftime(time(0), i) < 36 ;) {

		bool ok = cap.grab();

		if (ok == false) {

			std::cout << "Video Capture Fail" << std::endl;


		}
		else {
			
			// obtain input image from source
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			// Just resize input image if you want
			//resize(img, img, Size(640, 480));

			string text1 = "Please step out of frame";
			string text2 = "About to take photo";
			string text3 = "Step back into frame with guitar";
			string text4 = "NOTE";
			int fontFace = FONT_HERSHEY_PLAIN;
			double fontScale = 2;
			int thickness = 3;
			cv::Point textOrg(100, 130);
			

			// create foreground mask of proper size
			if (foregroundMask.empty()) {
				foregroundMask.create(img.size(), img.type());
			}

			// compute foreground mask 8 bit image
			// -1 is parameter that chose automatically your learning rate

			bg_model->operator()(img, foregroundMask, true ? -1 : 0);

			// smooth the mask to reduce noise in image
			GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);

			// threshold mask to saturate at black and white values
			threshold(foregroundMask, foregroundMask, 10, 255, THRESH_BINARY);


			// create black foreground image
				foregroundImg = Scalar::all(0);
			// Copy source image to foreground image only in area with white mask
			img.copyTo(foregroundImg, foregroundMask);

			//Get background image
			bg_model->getBackgroundImage(backgroundImage);

			// Show the results
			imshow("foreground mask", foregroundMask);
			imshow("foreground image", foregroundImg);

			int key6 = waitKey(40);

			if (!backgroundImage.empty()) {

				//imshow("mean background image", backgroundImage);
				//int key5 = waitKey(40);

			}
			secs = difftime(time(0), start);
			if (secs < 5)
			{
				cv::putText(img, text1, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
			}
			else if (secs >= 5 && secs < 10)
			{
				cv::putText(img, text2, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
			}
			else if (secs == 10)
			{
				imwrite("1_w.png", img);
				PlaySound(TEXT("hello.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (secs > 10 && secs < 17)
			{
				cv::putText(img, text3, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
			}
			else if (secs == 17)
			{
				PlaySound(TEXT("pop.wav"), NULL, SND_FILENAME);
				cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
				string filename = "2_";
				filename += std::to_string(secs/15);
				filename += ".png";
				imwrite("2_w.png", foregroundMask);
				imwrite(filename, foregroundImg);
			}
			else if (secs % 3 == 2)
			{
				PlaySound(TEXT("pop.wav"), NULL, SND_FILENAME);
				cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
				string filename = "2_";
				filename += std::to_string(secs);
				filename += ".png";
				imwrite(filename, img);
			}
			imshow("Guitar Note Detector", foregroundImg);

		}

	}
}

bool record()
{
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return false;
	namedWindow("Guitar Note Detector", 1);
	int i = 0;
	for (;;)
	{
		Mat frame;
		cap >> frame; // 25 FPS
		string text1 = "Please step out of frame";
		string text2 = "About to take photo";
		string text3 = "Step back into frame with guitar";
		int fontFace = FONT_HERSHEY_PLAIN;
		double fontScale = 2;
		int thickness = 3;
		cv::Point textOrg(100, 130);
		if (i < 200)
		{
			cv::putText(frame, text1, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (i > 200 && i < 225)
		{
			cv::putText(frame, text2, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (i == 225)
		{
			imwrite("1_w.png", frame);
		}
		else if (i > 225 && i < 425)
		{
			cv::putText(frame, text3, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (i > 425 && i < 450)
		{
			cv::putText(frame, text2, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		else if (i == 450)
		{
			imwrite("2_w.png", frame);
		}
		i++;
		imshow("Guitar Note Detector", frame);
		if (waitKey(30) >= 0) break;
	}
	// the camera will be deinitialized automatically in VideoCapture destructor
	return true;
}
	
