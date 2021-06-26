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
#include "Area.h"


/*
  std::string type = type_of_Mat(image.type());
  printf("Matrix: %s %dx%d \n", type.c_str(), image.cols, image.rows);
*/
int mode = 3;
int blur = 9;
int blur_1 = 1;
int lowThresholdS = 27; //70
int lowThresholdV = 90; //34
int lowThresholdH = 40; //50
int lowThresholdExt = 205;



void find_robot(cv::Mat& frame)
{
	find_QR(frame);
	find_tip(frame);
	findRect(boundRectQR, boundRectTip);

	for (int i = 0; i < boundRectTemp.size(); ++i)
	{
		cv::rectangle(frame, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
	}

	imorientation(frame, boundRectTemp);

	boundRectTemp.clear();
	cv::imshow("origin", frame);

	cv::waitKey(0);
	cv::destroyAllWindows();
}

void fcheus(cv::Mat& frame)
{
	cv::Mat imgBlur, temp, imgCanny, imgCanny2, imgCanny3, imgCanny4, imgRes, imgDel, res, imgHist, mask, origin;
	std::vector<cv::Mat> channelsRGB, channelsHSV, channelsRGBHist, imgCannys;
	int contrSize = 2500;
	int errRate = 200;
	lowThreshold = 100;
	std::vector<us::BGR> ignCol;
	ignCol.push_back({170, 60, 200});
	//us::Areas Areas(ignCol);
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));

	us::makeBorder(frame, temp, 0.25);

	cv::Rect roi(0, frame.rows * 0.25, frame.cols, frame.rows - frame.rows * 0.25);
	cv::Mat imgCrop = frame(roi);


	cv::resize(imgCrop, origin, cv::Size(frame.cols * 0.5, frame.rows * 0.5));
	cv::imshow("original", origin);
	imgCrop = origin.clone();
	imgRes = origin.clone();

	channelsRGB = getChannels(imgRes, 1, 0);

	cv::cvtColor(imgRes, temp, cv::COLOR_BGR2HSV);
	//cv::imshow("HSV", temp);

	channelsHSV = getChannels(temp, 2, 0);

	us::equalizeHist_BGR(imgRes, imgHist);
	cv::imshow("equalizeHist_BGR", imgHist);
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

	
	cv::medianBlur(channelsRGBHist[0], imgBlur, blur);
	//cv::imshow("imgBlur - V", imgBlur);
	cv::Canny(imgBlur, imgDel, lowThresholdV, lowThresholdV * ratio, 3);
	cv::dilate(imgDel, imgCanny3, kernel);
	cv::imshow("imgCanny - V", imgCanny3);
	

	kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	us::findDominantColor(imgHist, imgDel);
	cv::imshow("imgHist", imgDel);
	cv::medianBlur(imgDel, mask, blur);
	cv::imshow("imgHist", mask);
	cv::Canny(mask, imgDel, lowThreshold, lowThreshold * ratio, 3);
	cv::dilate(imgDel, imgCanny4, kernel);
	cv::imshow("imgCanny - 4", imgCanny4);

	imgCannys.push_back(imgCanny);
	imgCannys.push_back(imgCanny2);
	imgCannys.push_back(imgCanny4);
	imgCannys.push_back(imgCanny3);
	imgCannys.push_back(imgCrop);

	us::findBorder(imgCannys, imgCrop, 1);

	cv::imshow("original + mask", imgCrop);

	cv::medianBlur(imgHist, imgBlur, blur_1);


	cv::Canny(imgBlur, imgCanny, lowThresholdExt, lowThresholdExt * ratio, 3);
	
	cv::Point3_<uchar>* p, *h, *c;

	//cv::imshow("imgCrop", imgCrop);
	
	for (int i = 0; i < imgHist.rows; ++i)
	{
		for (int j = 0; j < imgHist.cols; ++j)
		{
			p = imgCrop.ptr<cv::Point3_<uchar> >(i, j);
			h = origin.ptr<cv::Point3_<uchar> >(i, j);
			c = imgCanny.ptr<cv::Point3_<uchar> >(i, j);
			if ((p->x || p->y || p->z) > 0)
			{
				h->x = 170;
				h->y = 60;
				h->z = 200;
				c->x = 255;
				c->y = 255;
				c->z = 255;
				p->x = 255;
				p->y = 255;
				p->z = 255;
			}
		}
	}
	cv::imshow("mask", origin);
	cv::imshow("channelsRGBHist", imgCanny);
	cv::imshow("imgCrop", imgCrop);
	//Areas.FindAreas(mask, res);
	
	res = imgCrop.clone();
	cv::dilate(res, imgCrop, kernel);

	us::Areas Areas;
	cv::Point point;
	bool SuccesCheck = 1;
	us::BGR color;
	/*
	while (SuccesCheck)
	{
		SuccesCheck = Areas.checkAreas(imgCrop, point);
		int x = point.x;
		int y = point.y;

		color.B = 1 + rand() % 254;
		color.G = 1 + rand() % 254;
		color.R = 1 + rand() % 254;

		//us::floodFill4(imgCrop, x, y, color);
		//Areas.fillArea(imgCrop, res, x, y);
		cv::imshow("imgCrop", res);
		imgCrop = res.clone();
		cv::waitKey(0);
	}

	cv::imshow("imgCrop", imgCrop);
	*/


	//us::markArea(imgCrop, imgRes);
//	cv::imshow("mask", mask);
//	cv::imshow("mask + Canny", imgCanny);

	//createHistogram(frame, res, mode);
	//cv::imshow("Histogram - 2", res);

	//cv::namedWindow("Trackbars", (640, 200));
	//cv::createTrackbar("Mode", "Trackbars", &mode, 3);

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
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("blur_1", "Trackbars", &blur_1, 21);
	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("lowThresholdExt", "Trackbars", &lowThresholdExt, 300);

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
	
	us::isVideo(video_name, 10, fcheus, 0);
	//us::isPhoto(file_name, func);
	
	return 0;
}
