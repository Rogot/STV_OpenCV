#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>


// for storing RGB channels
cv::Mat rgb;
cv::Mat channel_rgb[3];
// for storing RGB channels after conversion
cv::Mat channel_rgb_range[3];
// for storing total page
cv::Mat rgb_and;

//Calculate histogramm
int* getHistogram(const cv::Mat arr)
{
	const int size_h = 256;

	//make empty histogramm
	int* hist = new int[size_h]();

	for (int i = 0; i < arr.rows; ++i)
	{
		for (int j = 0; j < arr.cols; ++j)
		{
			hist[(int)arr.at<char>(i, j)]++;
		}
	}

	return hist;
}

float scan_sec(const cv::Mat& ImputArray, const cv::Mat& Mask, int x, int y) {
	int count = 0;
	int offset = 10;
	for (int i = 0; i < Mask.rows; ++i) {
		for (int j = 0; j < Mask.cols; ++j) {
			if (Mask.at<char>(i, j) <= ImputArray.at<char>(x + i, y + j) + offset && Mask.at<char>(i, j) >= ImputArray.at<char>(x + i, y + j) - offset)
				//if (ImputArray.at<char>(x + i, y + j) + offset <= Mask.at<char>(i, j) && ImputArray.at<char>(x + i, y + j) - offset >= Mask.at<char>(i, j))
				//if (ImputArray.at<char>(x + i, y + j) == Mask.at<char>(i, j))
			{
				count++;
			}
		}
	}
	return count / (Mask.rows * Mask.cols);
}

void find_Object(const cv::Mat& ImputArray, const cv::Mat& Mask, cv::Point& Point, float percent) {
	float res;
	for (int i = 0; i < ImputArray.rows / Mask.rows; ++i) {
		for (int j = 0; j < ImputArray.cols / Mask.cols; ++j) {
			Point.x = i * Mask.rows;
			Point.y = j * Mask.cols;
			res = scan_sec(ImputArray, Mask, Point.x, Point.y);
			if (res >= percent)
			{
				Point.x *= i;
				Point.y *= j;
				return;
			}
		}
	}
	//if image don't have this pattern
	Point.x = 0;
	Point.y = 0;
}

//Drow rectingle
void Draw(cv::Mat& image, const cv::Point& p, int weight, int height) {
	cv::Rect rect(p.x, p.y, weight, height);
	cv::rectangle(image, rect, cv::Scalar(0, 255, 0));
}

