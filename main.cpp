#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <fstream>

#include <iostream>

#include "extra.h"



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


int main(int argc, char** argv)
{
	std::ofstream file;
	file.open("test.txt");
	int arr_size = 256;
	int* arr;

	cv::Mat mat;
	cv::namedWindow("Demo", cv::WINDOW_AUTOSIZE);

	mat = cv::imread("Test_Robotino.jpg", cv::IMREAD_COLOR);
	
	std::string type = type_of_Mat(mat.type());
	printf("Matrix: %s %dx%d \n", type.c_str(), mat.cols, mat.rows);

	cv::imshow("Demo", mat);

	cv::waitKey(0);
	return 0;
}

