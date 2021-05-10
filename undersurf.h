#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <opencv2/objdetect.hpp>

#include <cmath>
#include <algorithm>
#include <assert.h>


void is_video(std::string file_name);
void is_video(std::string file_name, void* func);

void is_photo(std::string file_name);

/*
* num_ph - number of photos
*/
void is_photo(std::string file_name, int num_ph);





void is_video(std::string file_name) {

	cv::namedWindow("Video_file: " + file_name, cv::WINDOW_AUTOSIZE);

	// получаем информацию о видео-файле
	cv::VideoCapture cap(file_name);
	double fps = cap.get(cv::CAP_PROP_FPS);


	while (cap.isOpened()) {
		cv::Mat frame;
		bool bSuccess = cap.read(frame);

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		cv::imshow("Cap", frame);

		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}

	}

	cv::destroyAllWindows();
}

template <typename T>
void is_video(std::string file_name, T* func) {

	cv::namedWindow("Video_file: " + file_name, cv::WINDOW_AUTOSIZE);

	// get information about video file
	cv::VideoCapture cap(file_name);
	double fps = cap.get(cv::CAP_PROP_FPS);


	while (cap.isOpened()) {
		cv::Mat frame;
		bool bSuccess = cap.read(frame);

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		func(&frame);

		cv::imshow("Cap", frame);

		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}

	}

	cv::destroyAllWindows();
}


void is_photo(std::string file_name) {
	
	cv::Mat image;

	image = imread(file_name, cv::IMREAD_COLOR);
	find_QR(image);
	find_tip(image);
	findRect(boundRectQR, boundRectTip);

	for (int i = 0; i < boundRectTemp.size(); ++i)
	{
		cv::rectangle(image, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
		imorientation(image, boundRectTemp);

		boundRectTemp.clear();
		cv::imshow("origin " + std::to_string(i), image);

		cv::waitKey(0);
		cv::destroyAllWindows();
	}
}

void is_photo(std::string file_name, int num_ph) {

	cv::Mat image;

	for (int i = 1; i <= num_ph; ++i)
	{
		file_name = "./Test_photo/ct" + std::to_string(i) + ".jpg";

		image = imread(file_name, cv::IMREAD_COLOR);
		find_QR(image);
		find_tip(image);
		findRect(boundRectQR, boundRectTip);

		for (int i = 0; i < boundRectTemp.size(); ++i)
		{
			cv::rectangle(image, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
			imorientation(image, boundRectTemp);

			boundRectTemp.clear();
			cv::imshow("origin " + std::to_string(i), image);

			cv::waitKey(0);
			cv::destroyAllWindows();
		}

	}
}