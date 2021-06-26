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


//for finding QR
cv::Mat imgGray, imgBlur, imgCanny, imgDil;
int lowThreshold = 25;
float ratio = 3;

//for fanding orange
cv::Mat imgHSV, mask;
int hmin = 11, smin = 121, vmin = 151;
int hmax = 179, smax = 186, vmax = 255;
cv::Scalar lower(hmin, smin, vmin);
cv::Scalar upper(hmax, smax, vmax);

//¬естор с координатами всех контуров с QR и цветным наконечником
std::vector<cv::Rect> boundRectQR, boundRectTip, boundRectTemp;
std::vector<cv::Rect> boundRectTemp2;
cv::Mat dst;
int thresh_val = 0;
int thresh_type = 0;
float angle;

#define NORMAL 0
#define NEGATIVE_VAL 1


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
	}
	if (boundRectTip.size() != 0) {
		boundRectTemp.push_back(boundRectTip[numTip]);
	}
}

//Find coordinate of point
void findCoord(cv::Point p, cv::Point& pOut, float scale) {
	
	if (scale <= 1 && scale > 0)
	{
		pOut.x = p.x * scale;
		pOut.y = p.y * scale;
	}
	else {
		assert(scale > 1 && scale <= 0);
	}
}

/*
* p1 - координата QR
* p2 - координата наконечника
* mode - выбор режима отображени€ углов больше 180 градусов
*/
float findOrient(cv::Point p1, cv::Point p2, int mode) {
	
	//Ќайдем угол ч/з теорему косинусов
	//а - сторона треугольника, напротив искомого угла 
	//sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
	float a = p1.x - p2.x;
	float b = p1.y - p2.y;
	float c = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

	//float t = (pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c);

	float angl = acos((pow(b, 2) + pow(c, 2) - pow(a, 2)) / (2 * b * c)) * 180 / 3.14;

	if (p1.x < p2.x)
	{
		if (mode == NORMAL)
		{
			angl = 180 + angl;
		}
		if (mode == NEGATIVE_VAL) {
			angl -= 2 * angl;
		}
	}
	else
	{
		angl = 180 - angl;
	}

	return angl;
}

void imorientation(cv::Mat img, std::vector<cv::Rect> boundRect) {
	cv::Point p, pOut, p1,pOut1;
	p = findCentr(boundRect[0]);
	findCoord(p, pOut, 1);
	//cv::circle(img, p, 5, cv::Scalar(0, 0, 255));
	cv::putText(img, "Coordinate", { boundRect[0].x, boundRect[0].y - 50 }, cv::FONT_HERSHEY_DUPLEX, 0.55, cv::Scalar(0, 255, 102), 2, 1);
	cv::putText(img, "x: " + std::to_string(pOut.x) + 
		" y: " + std::to_string(pOut.y),
		{ boundRect[0].x, boundRect[0].y - 30 }, cv::FONT_HERSHEY_DUPLEX, 0.55, cv::Scalar(0, 255, 102), 2, 1);
	if (boundRect.size() == 2) {
		p1 = findCentr(boundRect[1]);
		//cv::circle(img, p1, 5, cv::Scalar(255, 0, 0));
		angle = findOrient(p, p1, NORMAL);
		cv::putText(img, "Angle: " + std::to_string(angle), { boundRect[0].x, boundRect[0].y - 10 }, cv::FONT_HERSHEY_DUPLEX, 0.55, cv::Scalar(0, 255, 102), 2, 1);
		angle = 0;
	}

}

//Find contours and drow it in the image
std::vector<cv::Rect> getContours(cv::Mat imgDil, cv::Mat img, int numPix, float errRate, float koeff) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());
	
	for (int i = 0; i < contours.size(); ++i)
	{
		//ќпредел€ем размер контуров
		int area = cv::contourArea(contours[i]);

		if (area > numPix - errRate && area < numPix + errRate)
		{
			float peri = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], koeff * peri, true);

			boundRect[i] = boundingRect(conPoly[i]);
		}
	}
	return boundRect;
}

//Find contours and drow it in the image with countrs
std::vector<cv::Rect> getContours(cv::Mat imgDil, cv::Mat img, int numPix, float errRate, float koeff, int conturs, int countur_size) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	std::vector<std::vector<cv::Point>> conPoly(contours.size());
	std::vector<cv::Rect> boundRect(contours.size());

	for (int i = 0; i < contours.size(); ++i)
	{
		//ќпредел€ем размер контуров
		int area = cv::contourArea(contours[i]);


		//if (area > 0 && area < 8000)
		if (area > numPix - errRate && area < numPix + errRate)
		{
			float peri = cv::arcLength(contours[i], true);
			cv::approxPolyDP(contours[i], conPoly[i], koeff * peri, true);
			if (conturs == 1 || conturs == 2) {
				cv::drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), countur_size);
				
				std::cout << area << std::endl;
				if (conturs == 2) {
					cv::imshow("TEST", img);
					cv::waitKey(0);
				}
			}

			boundRect[i] = boundingRect(conPoly[i]);
		}
	}
	return boundRect;
}


/********* Useless functions now *********/
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
	cv::blur(imgGray, imgBlur, cv::Size(2, 2));
	cv::Canny(imgGray, imgCanny, lowThreshold, lowThreshold * ratio, 3);

	//cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	//cv::dilate(imgCanny, imgDil, kernel);

	boundRectTemp = getContours(imgCanny, img, 2500, 300, 0.001);
}

//Find orange tip
void find_tip(cv::Mat img) {
	cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
	cv::inRange(imgHSV, lower, upper, mask);
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(mask, imgDil, kernel);
	//	cv::imshow("imgDil",imgDil);

	//ќпераци€ наращивани€ дл€ соединени€ разделенных областей одного объекта
	//kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(imgDil, dst, kernel);
	//cv::imshow("imgDilAfter", dst);

	boundRectTemp = getContours(dst, img, 600, 300, 0.02);
}

//Drow rectingle
void Draw(cv::Mat& image, const cv::Point& p, int weight, int height) {
	cv::Rect rect(p.x, p.y, weight, height);
	cv::rectangle(image, rect, cv::Scalar(0, 255, 0));
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


void createHistogram(const cv::Mat& input, cv::Mat& output) {
	
	std::vector<cv::Mat> channels;
	cv::split(input, channels);

	int histSize = 256; //from 0 to 255

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	cv::Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&channels[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for R, G and B
	int hist_w = 512; int hist_h = 400;

	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	for (int i = 1; i < histSize; i++)
	{
		line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
		line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
			cv::Scalar(0, 255, 0), 2, 8, 0);
		line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}

	output = histImage.clone();
}

void createHistogram(const cv::Mat& input, cv::Mat& output, int mode) {

	std::vector<cv::Mat> channels;
	cv::split(input, channels);

	int histSize = 256; //from 0 to 255

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 }; //the upper boundary is exclusive
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	cv::Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&channels[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&channels[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for R, G and B
	int hist_w = 512; int hist_h = 400;

	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	/// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	for (int i = 1; i < histSize; i++)
	{
		if (mode == 0 || mode == 3)
		{
			line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))),
				cv::Scalar(255, 0, 0), 2, 8, 0);
		}
		if (mode == 1 || mode == 3)
		{
			line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))),
				cv::Scalar(0, 255, 0), 2, 8, 0);
		}
		if (mode == 2 || mode == 3)
		{
			line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
				cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))),
				cv::Scalar(0, 0, 255), 2, 8, 0);
		}
	}

	output = histImage.clone();
}

std::vector<cv::Mat> getChannels(const cv::Mat& input, int mode, bool print)
{
	std::vector<cv::Mat> channels;
	cv::split(input, channels);

	if (print) {
		if (mode == 1) {
			cv::imshow("B", channels[0]);
			cv::imshow("G", channels[1]);
			cv::imshow("R", channels[2]);
		}
		else if (mode == 2) {
			cv::imshow("H", channels[0]);
			cv::imshow("S", channels[1]);
			cv::imshow("V", channels[2]);
		}
	}

	return channels;
}