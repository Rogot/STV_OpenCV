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
template <typename T>
void is_video(std::string file_name, T* func);
template <typename T>
void is_photo(std::string file_name, T* func);

/*
* num_ph - number of photos
*/
template <typename T>
void is_photo(std::string file_name, int num_ph, T* func);





void is_video(std::string file_name, int frame_rate) {

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
void is_video(std::string file_name, int frame_rate, T* func, bool IsShow) {

	cv::namedWindow("Video_file: " + file_name, cv::WINDOW_AUTOSIZE);

	// get information about video file
	cv::VideoCapture cap(file_name);
	double fps = cap.get(cv::CAP_PROP_FPS);
	int frame_num = 0;

	while (cap.isOpened()) {
		cv::Mat frame;
		bool bSuccess = cap.read(frame);
		frame_num++;

		//Breaking the while loop at the end of the video
		if (bSuccess == false)
		{
			std::cout << "Found the end of the video" << std::endl;
			break;
		}

		if (frame_num % (60/frame_rate) == 0) {
			if (IsShow) cv::imshow("Cap", frame);

			func(frame);

			if (IsShow) cv::imshow("Filtered", frame);
		}

		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}

		//cv::waitKey(0);

	}

	cv::destroyAllWindows();
}

template <typename T>
void is_photo(std::string file_name, T* func) {

	cv::Mat image;

	image = imread(file_name, cv::IMREAD_COLOR);

	func(&image);
}

template <typename T>
void is_photo(std::string file_name, int num_ph, T* func) {

	cv::Mat image;

	for (int i = 1; i <= num_ph; ++i)
	{
		file_name = "./Test_photo/ct" + std::to_string(i) + ".jpg";

		image = imread(file_name, cv::IMREAD_COLOR);
		cv::imshow("image", image);
		func(&image);
		cv::imshow("After", image);
	}
}