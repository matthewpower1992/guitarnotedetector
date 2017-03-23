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

void playNote(int num);
int findString(int stringY);
int neckWidth;
vector<Mat> notes;
Mat savedMask, neck;
vector<int> fretEnds;
vector<Point> fingerNotes;
int prevNum;

int main(int argc, char** argv)
{	
	Mat mask, img, maskedImg;	
	Rect neckROI;
	Mat rot_mat;
	prevNum = 0;

	//Background subtractor
	Ptr<BackgroundSubtractor> subtractor = new BackgroundSubtractorMOG2();

	//Open webcam capture and check if opened
	VideoCapture capture(0);
	if (!capture.isOpened())
		return false;

	//Create window to display webcam video
	namedWindow("Guitar Note Detector", 1);
	moveWindow("Guitar Note Detector", 0, 0);

	//Initialize timer
	time_t start = time(0);

	//Times for stages of calibration/capture for webcam
	int duration = 40;
	int maskCap = 13;
	int numNotes = 0;
	
	//Commence capturing
	for (;;)
	{
		//Retrieve frame from webcam
		if (!capture.grab())
		{
			std::cout << "Video failed to capture" << std::endl;
		}
		capture.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);

		//If within mask capture duration, add frame to background subtractor
		if (difftime(time(0), start) < maskCap)
		{
			if (mask.empty()) {
				mask.create(img.size(), img.type());
			}
			subtractor->operator()(img, mask, -1);
			GaussianBlur(mask, mask, Size(11, 11), 3.5, 3.5);
			threshold(mask, mask, 10, 255, THRESH_BINARY);
		}

		//Display mask and masked image (used for debugging purposes)
		maskedImg = Scalar::all(0);
		img.copyTo(maskedImg, mask);
		namedWindow("Foreground Mask", 1);
		moveWindow("Foreground Mask", 0, 500);
		imshow("Foreground Mask", mask);
		namedWindow("Foreground Image", 1);
		moveWindow("Foreground Image", 700, 500);
		imshow("Foreground Image", maskedImg);
		waitKey(40);

		//Text to display on screen 
		string text1 = "Please step out of frame";
		string text2 = "Calibrating";
		string text3 = "Step back into frame with guitar";
		string text4 = "Begin";
		string text5 = "Note captured";
		int fontFace = FONT_HERSHEY_PLAIN;
		double fontScale = 1;
		int thickness = 3;
		cv::Point textOrg(100, 130);

		//Times for calibration/capture time/rate
		int backgroundCalib = 3;
		int backgroundCap = 7;
		int noteRate = 3;

		//Seconds from start
		int seconds = difftime(time(0), start);

		if (seconds <= maskCap)
		{
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
				//Capture mask, detect neck and fret positions
				neck = detectGuitarNeck(maskedImg, img, neckWidth, rot_mat, neckROI);
				fretEnds = fretDetector(neck);
				cv::putText(img, text4, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);

			}
		}		
		else if (seconds % noteRate == (noteRate - 1))
		{	
			//At initerval dictated by noteRate, capture frame and determine note being played
			Point center = Point(img.cols / 2, img.rows / 2);
			Mat rotatedImage;
			warpAffine(img, rotatedImage, rot_mat, img.size());

			//Crop neck based on ROI determined in original masked image
			rotatedImage(neckROI).copyTo(neck);

			//Initialized number of detected notes to 0, find fingertip locations
			int num = 0;
			fingerNotes = findFingertips(neck, num);

			//If fingertips found, play note(s)
			if (num > 0)
			{
				playNote(num);

			}
			cv::putText(img, text5, textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
		}
		imshow("Guitar Note Detector", img);
	}

	return 0;
}

void playNote(int num)
{
	//x and y coordinates of fingertip
	int fretX, stringY;

	//get number of notes already played
	int temp = prevNum;
	prevNum = num;

	//Determine note being played and play sound associated with that note
	for (int i = temp ;i < num;i++)
	{
		fretX = fingerNotes[i].x;
		stringY = findString(fingerNotes[i].y);
		if (fretX > fretEnds[0])
		{
			if (stringY == 6)
			{
				PlaySound(TEXT("Notes/HE1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is HE1" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 5)
			{
				PlaySound(TEXT("Notes/B1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is B1" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 4)
			{
				PlaySound(TEXT("Notes/G1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is G1" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 3)
			{
				PlaySound(TEXT("Notes/D1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is D1" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 2)
			{
				PlaySound(TEXT("Notes/A1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is A1" << std::endl;
				waitKey(40);
				return;
			}
			else
			{
				PlaySound(TEXT("Notes/E1.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is E1" << std::endl;
				waitKey(40);
				return;
			}

		}
		else if (fretX > fretEnds[1])
		{
			if (stringY == 6)
			{
				PlaySound(TEXT("Notes/HE2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is HE2" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 5)
			{
				PlaySound(TEXT("Notes/B2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is B2" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 4)
			{
				PlaySound(TEXT("Notes/G2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is G2" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 3)
			{
				PlaySound(TEXT("Notes/D2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is D2" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 2)
			{
				PlaySound(TEXT("Notes/A2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is A2" << std::endl;
				waitKey(40);
				return;
			}
			else
			{
				PlaySound(TEXT("Notes/E2.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is E2" << std::endl;
				waitKey(40);
				return;
			}
		}
		else if (fretX > fretEnds[2])
		{
			if (stringY == 6)
			{
				PlaySound(TEXT("Notes/HE3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is HE3" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 5)
			{
				PlaySound(TEXT("Notes/B3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is B3" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 4)
			{
				PlaySound(TEXT("Notes/G3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is G3" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 3)
			{
				PlaySound(TEXT("Notes/D3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is D3" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 2)
			{
				PlaySound(TEXT("Notes/A3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is A3" << std::endl;
				waitKey(40);
				return;
			}
			else
			{
				PlaySound(TEXT("Notes/E3.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is E3" << std::endl;
				waitKey(40);
				return;
			}
		}
		else
		{
			if (stringY == 6)
			{
				PlaySound(TEXT("Notes/HE4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is HE4" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 5)
			{
				PlaySound(TEXT("Notes/B4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is B4" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 4)
			{
				PlaySound(TEXT("Notes/G4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is G4" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 3)
			{
				PlaySound(TEXT("Notes/D4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is D4" << std::endl;
				waitKey(40);
				return;
			}
			else if (stringY == 2)
			{
				PlaySound(TEXT("Notes/A4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is A4" << std::endl;
				waitKey(40);
				return;
			}
			else
			{
				PlaySound(TEXT("Notes/E4.wav"), NULL, SND_FILENAME);
				std::cout << "Note played is E4" << std::endl;
				waitKey(40);
				return;
			}
		}
	}
}

int findString(int stringY)
{
	//Divide overall neckwidth by 7 to get distance between strings
	double stringDiv = neckWidth / 7;
	int stringFound = (int)stringY / stringDiv;
	if (stringFound >= 6)
	{
		return 6;
	}
	else if (stringFound <= 1)
	{
		return 1;
	}
	return stringFound;
}