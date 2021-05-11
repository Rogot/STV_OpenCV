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




void find_robot(cv::Mat* frame)
{
	find_QR(*frame);
	find_tip(*frame);
	findRect(boundRectQR, boundRectTip);

	for (int i = 0; i < boundRectTemp.size(); ++i)
	{
		cv::rectangle(*frame, boundRectTemp[i].tl(), boundRectTemp[i].br(), cv::Scalar(0, 255, 0), 2);
		imorientation(*frame, boundRectTemp);

		boundRectTemp.clear();
		cv::imshow("origin " + std::to_string(i), *frame);

		cv::waitKey(0);
		cv::destroyAllWindows();
	}
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


	void func(cv::Mat& frame)
	{
		cv::Mat blur;
		cv::Mat temp;

		cv::imshow("original", frame);

		us::equalizeHist_BGR(frame, blur);
		cv::imshow("equalizeHist_BGR", blur);

		cv::medianBlur(blur, frame, 3);
		cv::imshow("equalizeHist_BGR + MedianFilter", frame);


		//cv::namedWindow("Trackbars", (640, 200));
		//cv::createTrackbar("Threshold", "Trackbars", &lowThreshold, 179);

		//cv::medianBlur(*frame, blur, 3);

		//cv::Canny(blur, *frame, lowThreshold, lowThreshold * ratio, 3);
		//cv::dilate(temp, *frame, kernel);
		//cv::cvtColor(blur, *frame, cv::COLOR_RGB2HSV);



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

		cv::inRange(*frame, lower, upper, mask);
		*frame = mask;
		*/
	}
}


int main()
{
	int num_ph = 31;
	std::string file_name, video_name;
	video_name = "./Test_photo/Second/Captured_file_2.avi";
	file_name = "./Test_photo/ct1.jpg";

	is_video(video_name, 10, *us::func, 0);
	//is_photo(file_name, num_ph, *find_robot);
	
	return 0;
}