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
bool capture();

int main(int argc, char** argv)
{
	capture();
	waitKey(0);
	//backsub();
	//waitKey(0);
	//DONT DILATE IN VIDEO...TEST HOUGH WITH BACKGROUND?
	Mat mask, guitarist, maskedImage, guitarNeck, fingerTips, frets;
	mask = imread("2_mask.png");
	guitarist = imread("2_35.png");
	guitarist.copyTo(maskedImage, mask);
	guitarNeck = detectGuitarNeck(maskedImage, guitarist);
	frets = fretDetector(guitarNeck);
	fingerTips = findFingertips(guitarNeck);
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

void backsub()
{
	// Init background substractor
	Ptr<BackgroundSubtractor> bg_model = new BackgroundSubtractorMOG2();

	// Create empy input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;

	// capture video from source 0, which is web camera, If you want capture video from file just replace //by  VideoCapture cap("videoFile.mov")
	VideoCapture cap(0);
	namedWindow("Guitar Note Detector", 1);
	moveWindow("Guitar Note Detector", 0, 0);
	namedWindow("foreground mask", 1);
	moveWindow("foreground mask", 700, 0);
	namedWindow("foreground image", 1);
	moveWindow("foreground image", 700, 500);
	time_t start = time(0);
	int secs;
	// main loop to grab sequence of input files
	for (int i = start; difftime(time(0), i) < 40 ;) {

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
			
			if (difftime(time(0), start) < 20)
			{
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
				//imshow("foreground mask", foregroundMask);
				//imshow("foreground image", foregroundImg);

				int key6 = waitKey(40);

				if (!backgroundImage.empty()) {

					//imshow("mean background image", backgroundImage);
					//int key5 = waitKey(40);

				}
			}
			foregroundImg = Scalar::all(0);
			// Copy source image to foreground image only in area with white mask
			img.copyTo(foregroundImg, foregroundMask);
			imshow("foreground mask", foregroundMask);
			imshow("foreground image", foregroundImg);

			int key6 = waitKey(40);
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
				PlaySound(TEXT("hello.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (secs > 10 && secs < 20)
			{
				cv::putText(img, text3, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
			}
			else if (secs == 20)
			{
				PlaySound(TEXT("pop.wav"), NULL, SND_FILENAME);
				cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
				string filename = "2_";
				filename += std::to_string(secs);
				string maskname = "2_mask.png";
				filename += ".png";
				imwrite(filename, img);
				imwrite(maskname, foregroundMask);

			}
			else if (secs % 3 == 2)
			{
				PlaySound(TEXT("pop.wav"), NULL, SND_FILENAME);
				cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
				string filename = "2_";
				filename += std::to_string(secs);
				//string maskname = filename + "mask.png";
				filename += ".png";
				imwrite(filename, img);
				//imwrite(maskname, foregroundMask);
			}
			imshow("Guitar Note Detector", img);

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
	
