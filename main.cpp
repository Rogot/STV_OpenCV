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
	cv::Mat imgBlur, temp, imgCanny, imgCanny2, imgRes, imgDel, res;
	int contrSize = 2500;
	int errRate = 200;
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	us::makeBorder(frame, temp, 0.25);

	cv::imshow("original", temp);

	cv::resize(temp, imgRes, cv::Size(frame.cols * 0.5, frame.rows * 0.5));

	//cv::Canny(imgRes, imgCanny, lowThreshold, lowThreshold * ratio, 3);
	//cv::imshow("imgCanny", imgCanny);

	us::equalizeHist_BGR(imgRes, frame);
	cv::imshow("equalizeHist_BGR", frame);

	us::markArea(frame, res, 3);
	cv::imshow("res", res);

	cv::medianBlur(res, imgBlur, 7);
	cv::imshow("Blur", imgBlur);

	cv::Canny(imgBlur, imgCanny, lowThreshold, lowThreshold * ratio, 3);
	cv::imshow("imgCanny", imgCanny);

	
	/*
	std::vector<cv::Mat> channels;
	cv::split(frame, channels);

	cv::Mat R, G, B;
	cv::Canny(channels[0], B, lowThreshold, lowThreshold * ratio, 3);
	cv::Canny(channels[1], G, lowThreshold, lowThreshold * ratio, 3);
	cv::Canny(channels[2], R, lowThreshold, lowThreshold * ratio, 3);

	cv::imshow("B", B);
	cv::imshow("G", G);
	cv::imshow("R", R);

	*/

	cv::namedWindow("Trackbars", (640, 200));
	cv::createTrackbar("Threshold", "Trackbars", &lowThreshold, 150);

	//cv::medianBlur(frame, blur, 3);

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
	file_name = "./Test_photo/ct1.jpg";
	
	us::isVideo(video_name, 10, func, 0);
	//us::isPhoto(file_name, num_ph, find_robot);
	
	return 0;
}
