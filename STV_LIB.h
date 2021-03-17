#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <cmath>
#include <algorithm>


// for storing RGB channels
cv::Mat rgb;
cv::Mat channel_rgb[3];
// for storing RGB channels after conversion
cv::Mat channel_rgb_range[3];
// for storing total page
cv::Mat rgb_and;

//for finding QR
cv::Mat imgGray, imgBlur, imgCanny, imgDil;
int lowThreshold = 65;
float ratio = 3;

//for fanding orange
cv::Mat imgHSV, mask;
int hmin = 11, smin = 121, vmin = 151;
int hmax = 179, smax = 186, vmax = 255;
cv::Scalar lower(hmin, smin, vmin);
cv::Scalar upper(hmax, smax, vmax);

//Вестор с координатами всех контуров с QR и цветным наконечником
std::vector<cv::Rect> boundRectQR, boundRectTip, boundRectTemp;
int countRec = 0;


//Find center of rectangle
cv::Point findCentr(cv::Rect boundRect) {

	cv::Point p;
	p.x = (boundRect.br().x - boundRect.tl().x) / 2 + boundRect.tl().x;
	p.y = (boundRect.br().y - boundRect.tl().y) / 2 + boundRect.tl().y;

	return p;
}

//Find the coordinates of the tip and QR
void findRect(std::vector<cv::Rect> boundRectQR, std::vector<cv::Rect> boundRectTip) {
	
	float dist = 0;
	float distMin = 0;

	int numQR = 0, numTip = 0, zap = 0;

	cv::Point p1, p2;

	for (int i = 0; i < boundRectQR.size(); ++i) {
		for (int j = 0; j < boundRectTip.size(); ++j) {
			
			p1 = findCentr(boundRectQR[i]);
			p2 = findCentr(boundRectTip[j]);

			dist = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

			if (distMin == 0 && dist > 0)
			{
				distMin = dist;
			}

			if (dist < distMin && dist > 0) {
				distMin = dist;
				numQR = i;
				numTip = j;
			}
			
		}
	}

	boundRectTemp.clear();
	if (boundRectQR.size() != 0) {
		boundRectTemp.push_back(boundRectQR[numQR]);
		countRec++;
	}
	if (boundRectTip.size() != 0) {
		boundRectTemp.push_back(boundRectTip[numTip]);
		countRec++;
	}
}


void imorientation() {
	cv::Point p1, p2;
	p1 = findCentr(boundRectTemp[0]);
	p2 = findCentr(boundRectTemp[1]);



}

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

//Find contours and drow it in the image
std::vector<cv::Rect> getContours(cv::Mat imgDil, cv::Mat img, int numPix, float errRate, float koeff) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	//std::string objType;

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());
	//cv::drawContours(img, contours, -1, cv::Scalar(255,0,255),2);

	for (int i = 0; i < contours.size(); ++i)
	{
		//Определяем размер контуров
		int area = cv::contourArea(contours[i]);


		//if (area > 100 && area < 800)
		if (area > numPix - errRate && area < numPix + errRate)
		{
			float peri = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], koeff * peri, true);
			//cv::drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 2);

			boundRect[i] = boundingRect(conPoly[i]);

			//cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 2);
			//cv::putText(img, objType, { boundRect[i].x, boundRect[i].y - 5 }, cv::FONT_HERSHEY_DUPLEX, 0.75, cv::Scalar(100, 50, 200), 2, 1);
			std::cout << area << std::endl;

			//cv::imshow("TEST", img);
			cv::waitKey(0);
		}
	}
	return boundRect;
}


void clear_vect(std::vector<cv::Rect> boundRectTemp, std::vector<cv::Rect>& boundRect) {

	boundRect.clear();
	std::vector<cv::Rect>::iterator it = boundRectTemp.begin();
	for (it; it < boundRectTemp.end(); ++it)
	{
		if (it->x > 0 || it->y > 0)
		{
			boundRect.push_back(*it);
		}
	}
}

//Find QR in the image
void find_QR(cv::Mat img)
{
	//Preprocessing
	cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);
	cv::blur(imgGray, imgBlur, cv::Size(3, 3));
	cv::Canny(imgGray, imgCanny, lowThreshold, lowThreshold * ratio, 3);

	//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::dilate(imgCanny, imgDil, kernel);

	boundRectTemp = getContours(imgCanny, img, 2500, 300, 0.001);
	clear_vect(boundRectTemp, boundRectQR);
	//boundRectTemp.clear();
}

//Find orange tip
void find_tip(cv::Mat img) {
	cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
	cv::inRange(imgHSV, lower, upper, mask);
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(mask, imgDil, kernel);
	boundRectTemp = getContours(imgDil, img, 310, 110, 0.02);
	clear_vect(boundRectTemp, boundRectTip);
	//boundRectTemp.clear();
}

//Drow rectingle
void Draw(cv::Mat& image, const cv::Point& p, int weight, int height) {
	cv::Rect rect(p.x, p.y, weight, height);
	cv::rectangle(image, rect, cv::Scalar(0, 255, 0));
}

