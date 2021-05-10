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

cv::Mat image;
cv::Mat QR;
cv::Mat dst2;
cv::Mat gray;
std::vector<cv::Mat> channel;

double minVal;
double maxVal;
cv::Point minLoc(0,0);
cv::Point maxLoc(0,0);

void func(cv::Mat* frame)
{
	cv::Mat temp = *frame;
	cv::Canny(temp, *frame, 65, 195, 3);
}


int main()
{
	int num_ph = 31;
	std::string file_name, video_name;
	video_name = "./Test_photo/Second/Captured_file_2.avi";
	file_name = "./Test_photo/ct1.jpg";

	is_video(video_name, *func);
	
	return 0;
}