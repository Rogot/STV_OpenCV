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
	std::string file_name, video_name;
	video_name = "./Test_photo/ct_cam.mp4";

	cv::namedWindow("Cap", cv::WINDOW_AUTOSIZE);

	// получаем информацию о видео-файле
	cv::VideoCapture cap(video_name);
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

		find_QR(frame);
		find_tip(frame);
		findRect(boundRectQR, boundRectTip);

		for (int i = 0; i < boundRectTemp.size(); ++i)
		{
			cv::rectangle(frame, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
		}

		imorientation(frame, boundRectTemp);

		boundRectTemp.clear();

		cv::imshow("Cap", frame);

		if (cv::waitKey(10) == 27)
		{
			std::cout << "Esc key is pressed by user. Stoppig the video" << std::endl;
			break;
		}

	}

	cv::destroyAllWindows();
	
	return 0;
}