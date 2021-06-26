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
#include "Area.h"


namespace Unit
{
	void decompozit(int number, std::vector<int>& divisors)
	{
		int divisor = 2;
		bool done = false;

		while (!done)
		{
			if (number % divisor == 0)
			{
				divisors.push_back(divisor);
				if (number / divisor == 1)
				{
					done = true;
				}
				number /= divisor;
			}
			else
			{
				divisor++;
			}
		}
	}

}

namespace us
{
	void isVideo(std::string file_name, int frame_rate);
	template <typename T>
	void isVideo(std::string file_name, int frame_rate, T* func, bool IsShow);
	template <typename T>
	void isPhoto(std::string file_name, T* func);

	/*
	* num_ph - number of photos
	*/
	template <typename T>
	void isPhoto(std::string file_name, int num_ph, T* func);


	void equalizeHist_BGR(const cv::Mat& input, cv::Mat& output);

	void markArea(const cv::Mat& input, cv::Mat& output);

	void markArea(const std::vector<cv::Mat> input, cv::Mat& output);
}

namespace us
{
	void equalizeHist_BGR(const cv::Mat& input, cv::Mat& output) {

		if (input.channels() < 3) return;

		std::vector<cv::Mat> channels;
		cv::split(input, channels);

		cv::Mat R, G, B;
		cv::equalizeHist(channels[0], B);
		cv::equalizeHist(channels[1], G);
		cv::equalizeHist(channels[2], R);

		std::vector<cv::Mat> combined;
		combined.push_back(B);
		combined.push_back(G);
		combined.push_back(R);

		cv::merge(combined, output);
	}

	void isVideo(std::string file_name, int frame_rate) {

		cv::namedWindow("Video_file: " + file_name, cv::WINDOW_AUTOSIZE);

		// Take information about video file
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

			if (frame_rate % (60 / frame_rate) == 0) {
				cv::imshow("Cap", frame);
			}

			if (cv::waitKey(10) == 27)
			{
				std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
				break;
			}

		}

		cv::destroyAllWindows();
	}

	template <typename T>
	void isVideo(std::string file_name, int frame_rate, T* func, bool IsShow) {

		//cv::namedWindow("Video_file: " + file_name, cv::WINDOW_AUTOSIZE);

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

			if (frame_num % (60 / frame_rate) == 0) {
				if (IsShow) cv::imshow("Cap", frame);

				func(frame);

				cv::waitKey(0);

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
	void isPhoto(std::string file_name, T* func) {

		cv::Mat image;

		image = imread(file_name, cv::IMREAD_COLOR);

		if (image.empty())
		{
			std::cout << "Picture didn't found" << std::endl;
			return;
		}

		func(image);

		cv::waitKey(0);
	}

	template <typename T>
	void isPhoto(std::string file_name, int num_ph, T* func) {

		cv::Mat image;

		for (int i = 1; i <= num_ph; ++i)
		{
			file_name = "./Test_photo/ct" + std::to_string(i) + ".jpg";

			image = imread(file_name, cv::IMREAD_COLOR);

			if (image.empty())
			{
				std::cout << "Picture didn't found" << std::endl;
				break;
			}

			cv::imshow("image", image);
			func(image);
			cv::imshow("After", image);
		}
	}

	void makeBorder(const cv::Mat& input, cv::Mat& output, float ratio) 
	{
		output = input.clone();

		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				if (i < ((int)input.rows * ratio))
				{
					// ��������� �������� 0-�� ������, �� ���� ������ ������
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 0) = 0;

					// ����������� �������� ������� ������, �� ���� �������� ������
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 1) = 0;

					// ����������� �������� �������� ������, �� ���� �������� ������
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 2) = 0;
				}
			}
		}
	}

	void img2bit(const cv::Mat& input, cv::Mat& output)
	{
		output = input.clone();
		cv::Point3_<uchar>* p;

		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				p = output.ptr<cv::Point3_<uchar> >(i, j);

				if ((p->x || p->y || p->z) > 0)
				{
					p->x = 255;
					p->y = 255;
					p->z = 255;
				}
			}
		}
	}

	void fillkernel(const cv::Mat& img, BGR** kernel, int rectSize, int curX, int curY)
	{
		//cv::Point3_<uchar>* p, *current;
		curX -= rectSize;
		curY -= rectSize;

		for (int i = 0; i < rectSize; ++i)
		{
			for (int j = 0; j < rectSize; ++j)
			{
				kernel[i][j].B = img.at<cv::Vec3b>(curY + i, curX + j)[0];
				kernel[i][j].G = img.at<cv::Vec3b>(curY + i, curX + j)[1];
				kernel[i][j].R = img.at<cv::Vec3b>(curY + i, curX + j)[2];
			}
		}
	}

	void findDominantColor(const cv::Mat& input, cv::Mat& output)
	{
		int b, g, r; //BGR
		int h, s, v; //HSV
		output = input.clone();
		cv::Point3_<uchar>* p;

		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				p = output.ptr<cv::Point3_<uchar> >(i, j);
				
				
				if (p->x > p->y && p->x > p->z)
				{
					//p->x = 255;
					p->y = 0;
					p->z = 0;
				}
				else if (p->z > p->x && p->z > p->y)
				{
					p->x = 0;
					p->y = 0;
					//p->z = 255;
				}
				else if (p->y > p->x && p->y > p->z)
				{
					p->x = 0;
					//p->y = 255;
					p->z = 0;
				}
				
			}
		}
	}

	void findBorder(const std::vector<cv::Mat> input, cv::Mat& output, bool mode)
	{
		int ext;
		int b, g, r; //BGR
		int h, s, v; //HSV
		output = input.back().clone();
		cv::Point3_<uchar>* p;

		for (int i = 0; i < input[0].rows; ++i)
		{
			for (int j = 0; j < input[0].cols; ++j)
			{
				h = input[0].at<bool>(i, j);
				s = input[1].at<bool>(i, j);
				v = input[2].at<bool>(i, j);
				ext = input[3].at<bool>(i, j);
				p = output.ptr<cv::Point3_<uchar> >(i, j);


				if (h == s && (h && s))
				{
					p->x = 255;
					p->y = 0;
					p->z = 0;
				}
				else if (h == v && (h && v))
				{
					p->x = 0;
					p->y = 255;
					p->z = 0;
				}
				else if (s == v && (v && s))
				{
					p->x = 0;
					p->y = 0;
					p->z = 255;
				}
				else if (s == ext && (ext && s))
				{
					p->x = 120;
					p->y = 120;
					p->z = 120;
				}
				else
				{
					if (mode) {
						p->x = 0;
						p->y = 0;
						p->z = 0;
					}
				}
				

			}
		}
	}

	void markArea(const cv::Mat& input, cv::Mat& output)
	{
		cv::Mat temp;
		temp = input.clone();
		cv::Point3_<uchar>* p, *pNext;

		img2bit(temp, output);
		
		cv::imshow("TEST - 1", output);

		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				if (j != input.cols - 1) {
					p = output.ptr<cv::Point3_<uchar> >(i, j);
					pNext = output.ptr<cv::Point3_<uchar> >(i, j + 1);

					if ((p->x && p->y && p->z) == 0 && (pNext->x < 255 && pNext->y < 255 && pNext->z < 255))
					{
						p->x = 200;
						p->y = 50;
						p->z = 10;
					}
					else if ((p->x && p->y && p->z) == 0 && (pNext->x == 255 && pNext->y == 255 && pNext->z == 255))
					{
						j = 0;
						if (i < input.rows)
							break;
					}

				}
			}
		}
		cv::imshow("TEST - 2", output);
	}

}