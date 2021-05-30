#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <string>

#include <iostream>

#include "STV_LIB.h"
#include "extra.h"
#include "undersurf.h"


/*
  std::string type = type_of_Mat(image.type());
  printf("Matrix: %s %dx%d \n", type.c_str(), image.cols, image.rows);
*/
int mode = 3;
int blur = 9;
int lowThresholdS = 27;
int lowThresholdV = 90; //34
int lowThresholdH = 40;



void find_robot(cv::Mat& frame)
{
	find_QR(frame);
	find_tip(frame);
	findRect(boundRectQR, boundRectTip);

	for (int i = 0; i < boundRectTemp.size(); ++i)
	{
		cv::rectangle(frame, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
		imorientation(frame, boundRectTemp);

		boundRectTemp.clear();
		cv::imshow("origin " + std::to_string(i), frame);

		cv::waitKey(0);
		cv::destroyAllWindows();
	}
}

void func(cv::Mat& frame)
{
	cv::Mat imgBlur, temp, imgCanny, imgCanny2, imgCanny3, imgCanny4, imgRes, imgDel, res, imgHist, mask;
	std::vector<cv::Mat> channelsRGB, channelsHSV, channelsRGBHist, imgCannys;
	int contrSize = 2500;
	int errRate = 200;
	lowThreshold = 100;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));

	us::makeBorder(frame, temp, 0.25);

	cv::Rect roi(0, frame.rows * 0.25, frame.cols, frame.rows - frame.rows * 0.25);
	cv::Mat imgCrop = frame(roi);


	cv::resize(imgCrop, imgRes, cv::Size(frame.cols * 0.5, frame.rows * 0.5));
	//cv::imshow("original", imgRes);
	imgCrop = imgRes.clone();

	//channelsRGB = showChannels(imgRes, 1);

	cv::cvtColor(imgRes, temp, cv::COLOR_BGR2HSV);
	cv::imshow("HSV", temp);

	channelsHSV = getChannels(temp, 2, 1);

	us::equalizeHist_BGR(imgRes, imgHist);
	//cv::imshow("equalizeHist_BGR", imgHist);
	channelsRGBHist = getChannels(imgHist, 1, 0);

	cv::medianBlur(channelsHSV[0], imgBlur, blur);
	//cv::imshow("imgBlur - H", imgBlur);
	cv::Canny(imgBlur, imgDel, lowThresholdH, lowThresholdH * ratio, 3);
	cv::dilate(imgDel, imgCanny, kernel);
	cv::imshow("imgCanny - H", imgCanny);

	cv::medianBlur(channelsHSV[1], imgBlur, blur);
	//cv::imshow("imgBlur - S", imgBlur);
	cv::Canny(imgBlur, imgDel, lowThresholdS, lowThresholdS * ratio, 3);
	cv::dilate(imgDel, imgCanny2, kernel);
	cv::imshow("imgCanny - S", imgCanny2);

	/*
	cv::medianBlur(channelsRGBHist[0], imgBlur, blur);
	cv::imshow("imgBlur - V", imgBlur);
	cv::Canny(imgBlur, imgDel, lowThresholdV, lowThresholdV * ratio, 3);
	cv::dilate(imgDel, imgCanny3, kernel);
	cv::imshow("imgCanny - V", imgCanny3);
	*/

	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	us::markArea(imgHist, imgDel);
	cv::medianBlur(imgDel, mask, blur);
	cv::imshow("imgHist", mask);
	cv::Canny(mask, imgDel, lowThreshold, lowThreshold * ratio, 3);
	cv::dilate(imgDel, imgCanny4, kernel);
	cv::imshow("imgCanny - 4", imgCanny4);

	imgCannys.push_back(imgCanny);
	imgCannys.push_back(imgCanny2);
//	imgCannys.push_back(imgCanny3);
	imgCannys.push_back(imgCanny4);
	imgCannys.push_back(imgCrop);

	us::markArea(imgCannys, imgCrop);



	cv::imshow("original + mask", imgCrop);
//	cv::imshow("mask", mask);
//	cv::imshow("mask + Canny", imgCanny);


	/*
	cv::imshow("H", channelsHSV[0]);
	cv::imshow("S", channelsHSV[1]);
	cv::imshow("V", channelsHSV[2]);
	cv::equalizeHist(channelsHSV[2], res);
	cv::imshow("V + hist", res);
	std::vector<cv::Mat> combined;
	combined.push_back(channelsHSV[0]);
	combined.push_back(channelsHSV[1]);
	combined.push_back(res);
	cv::merge(combined, res);
	cv::imshow("HSV + hist", res);
	*/

	/*
	cv::imshow("B", channelsRGB[0]);
	cv::imshow("G", channelsRGB[1]);
	cv::imshow("R", channelsRGB[2]);


	cv::imshow("B + hist", channelsRGBHist[0]);
	cv::imshow("G + hist", channelsRGBHist[1]);
	cv::imshow("R + hist", channelsRGBHist[2]);
	*/
	

	//createHistogram(frame, res, mode);
	//cv::imshow("Histogram - 2", res);

	//cv::namedWindow("Trackbars", (640, 200));
	//cv::createTrackbar("Mode", "Trackbars", &mode, 3);

	/*
	us::markArea(frame, res, 3);
	cv::imshow("res", res);

	cv::medianBlur(res, imgBlur, 15);
	cv::imshow("Blur", imgBlur);

	cv::Canny(imgBlur, imgCanny, lowThreshold, lowThreshold * ratio, 3);
	cv::imshow("imgCanny", imgCanny);
	*/

	//cv::Canny(imgRes, imgCanny, lowThreshold, lowThreshold * ratio, 3);
	//cv::imshow("imgCanny", imgCanny);

	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("Threshold", "Trackbars", &lowThreshold, 150);
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("lowThresholdS", "Trackbars", &lowThresholdS, 300);
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("lowThresholdV", "Trackbars", &lowThresholdV, 100);
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("lowThresholdH", "Trackbars", &lowThresholdH, 100);
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("Blure", "Trackbars", &blur, 21);

	//cv::dilate(temp, frame, kernel);
	//cv::cvtColor(blur, frame, cv::COLOR_RGB2HSV);

	/*
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);
	cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
	cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
	cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
	cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
	cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);

	cv::Scalar lower(hmin, smin, vmin);
	cv::Scalar upper(hmax, smax, vmax);

	cv::inRange(imgRes, lower, upper, mask);
	imgRes = mask;

	cv::imshow("HSV mask", imgRes);
	*/
	
}


int main()
{
	int num_ph = 31;
	std::string file_name, video_name;
	video_name = "./Test_photo/Second/Captured_file_2.avi";
	file_name = "./Test_photo/lena.jpg";
	
	us::isVideo(video_name, 10, func, 0);
	//us::isPhoto(file_name, func);
	
	return 0;
}
