#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <string>

#include <iostream>

#include "STV_LIB.h"
#include "extra.h"


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


int main()
{
	int num_ph = 31;
	std::string file_name;
	cv::Mat imgCrop;

	for (int i = 1; i <= num_ph; ++i)
	{
		file_name = "./Test_photo/ct" + std::to_string(i) + ".jpg";

		image = imread(file_name, cv::IMREAD_COLOR);

		find_QR(image);
		find_tip(image);
		findRect(boundRectQR, boundRectTip);

		//imgCrop = image(boundRectTemp[0]);

		//find(imgCrop);

		for (int i = 0; i < boundRectTemp.size(); ++i)
		{
			cv::rectangle(image, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
		}

		imorientation(image, boundRectTemp);

		boundRectTemp.clear();
		cv::imshow("origin " + std::to_string(i), image);

		cv::waitKey(0);
		cv::destroyAllWindows();
	}
	cv::destroyAllWindows();

	return 0;
}