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


void getContours(cv::Mat imgDil, cv::Mat img) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::string objType;

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());
	//cv::drawContours(img, contours, -1, cv::Scalar(255,0,255),2);

	for (int i = 0; i < contours.size(); ++i)
	{
		int area = cv::contourArea(contours[i]);



		if (area > 2500 - 300 && area < 2500 + 250) 
		{
			float peri = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], 0.001 * peri, true);
			cv::drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 2);
			//std::cout << conPoly[i].size() << std::endl;
			boundRect[i] = boundingRect(conPoly[i]);

			int objCor = (int)conPoly[i].size();

			if (objCor == 3) { objType = "Tri"; }
			else if (objCor == 4) { objType = "Rect"; }
			else if (objCor > 4) { objType = "Circle"; }

			cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 2);
			//cv::putText(img, objType, { boundRect[i].x, boundRect[i].y - 5 }, cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(100, 50, 200), 2, 1);
			std::cout << area << std::endl;

			cv::imshow("TEST", img);
			cv::waitKey(0);
		}
	}

}

//Drow rectingle
void Draw(cv::Mat& image, const cv::Point& p, int weight, int height) {
	cv::Rect rect(p.x, p.y, weight, height);
	cv::rectangle(image, rect, cv::Scalar(0, 255, 0));
}

