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


namespace Unit
{
	struct BGR {
		int B;
		int G;
		int R;
	};


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
	void imgReduct(const cv::Mat input, cv::Mat output, int ratio);
	void imgMagnif(const cv::Mat input, cv::Mat output, int ratio);

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
					// Назначаем операцию 0-му каналу, то есть синему каналу
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 0) = 0;

					// Присваиваем значение второму каналу, то есть зеленому каналу
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 1) = 0;

					// Присваиваем значение третьему каналу, то есть красному каналу
					*(output.data + output.step[0] * i + output.step[1] * j + output.elemSize1() * 2) = 0;
				}
			}
		}
	}

	void findColors(const cv::Mat& input, cv::Mat& output, int rectSize) {
		cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(rectSize, rectSize));
		std::vector<std::vector<cv::Point>> contours;

	}

	void fillkernel(const cv::Mat& img, Unit::BGR** kernel, int rectSize, int curX, int curY)
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

	void markArea(const cv::Mat& input, cv::Mat& output)
	{
		//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(rectSize, rectSize));
		//cv::Mat cloneInput = cv::getStructuringElement(cv::MORPH_RECT, 
		//	cv::Size(input.cols + (rectSize - 1), input.rows + (rectSize - 1)));
		//std::vector<cv::Point> markedPoints;
		//expBound(input, cloneInput, rectSize);

		/*
		int mediumB = 0, mediumG = 0, mediumR = 0;
		Unit::BGR** kernel;
		kernel = new Unit::BGR * [rectSize];
		for (int i = 0; i < rectSize; i++)
			kernel[i] = new Unit::BGR[rectSize];
			*/
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

	void markArea(const std::vector<cv::Mat> input, cv::Mat& output)
	{
		int mediumB = 0, mediumG = 0, mediumR = 0;
		Unit::BGR** kernel;

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
				//ext = input[3].at<bool>(i, j);
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
				/*else if (s == ext && (ext && s))
				{
					p->x = 120;
					p->y = 120;
					p->z = 120;
				}
				/*
				else
				{
					p->x = 0;
					p->y = 0;
					p->z = 0;
				}
				*/

			}
		}
	}
}


/*
void expBound(cv::Mat original, cv::Mat expended, int size)
{
	int startX = (size - 1) / 2;
	int startY = startX;
	cv::Point3_<int>* orig;
	cv::Point3_<int>* exp;

	for (int y = 0; y < original.rows; ++y)
	{
		for (int x = 0; x < original.cols; ++x)
		{
			orig = original.ptr<cv::Point3_<int> >(y, x);
			exp = expended.ptr<cv::Point3_<int> >(startY + y, startX + x);
			exp->x = orig->x;
			exp->y = orig->y;
			exp->z = orig->z;
		}
	}
	cv::imshow("TEST_original", original);
	cv::imshow("TEST_expended", expended);

	cv::waitKey(0);

	for (int y = 0; y < original.rows; ++y)
	{
		for (int x = 0; x < original.cols; ++x)
		{
			if (x < size || y < size) {
				orig = original.ptr<cv::Point3_<int> >(y + startY, x + startX);
				exp = expended.ptr<cv::Point3_<int> >(startY - y, startX - y);
			}
			else if (x > size + original.cols || y > size + original.rows) {
				orig = original.ptr<cv::Point3_<int> >(y - startY, x - startX);
				exp = expended.ptr<cv::Point3_<int> >(startY + y, startX + y);
			}
			exp->x = orig->x;
			exp->y = orig->y;
			exp->z = orig->z;

			cv::imshow("TEST_original", original);
			cv::imshow("TEST_expended", expended);

			cv::waitKey(0);

		}
	}

	cv::imshow("TEST_original", original);
	cv::imshow("TEST_expended", expended);
}
*/