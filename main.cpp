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
cv::Mat dst, dst2;
cv::Mat gray;
std::vector<cv::Mat> channel;

double minVal;
double maxVal;
cv::Point minLoc(0,0);
cv::Point maxLoc(0,0);

int hmin = 11, smin = 121, vmin = 151;
int hmax = 179, smax = 186, vmax = 255;


void getContours(cv::Mat imgDil, cv::Mat img) {

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgDil, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	//cv::drawContours(img, contours, -1, cv::Scalar(255,0,255),2);

	for (int i = 0; i < contours.size(); ++i)
	{
		int area = cv::contourArea(contours[i]);
		std::cout << area << std::endl;
		if (area > 1000)
		{
			cv::drawContours(img, contours, i, cv::Scalar(255, 0, 255), 2);
			cv::imshow("Counters", img);
			cv::waitKey(0);
			cv::destroyWindow("Counters");
		}
	}

}

int main()
{
	int num_ph = 19;
	std::string file_name = "./Test_photo/wt" + std::to_string(num_ph) + ".jpg";
	
	cv::Mat imgGray, imgBlur, imgCanny, imgDil;

	image = imread(file_name, cv::IMREAD_COLOR);

	//Preprocessing
	cv::cvtColor(image, imgGray, cv::COLOR_BGR2GRAY);
	cv::GaussianBlur(imgGray, imgBlur, cv::Size(3, 3), 3, 0);
	cv::Canny(imgBlur, imgCanny, 25,75);

	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(imgCanny, imgDil, kernel);

	getContours(imgDil, image);


	cv::imshow("origin", image);
	cv::imshow("Gaussian Blur", imgBlur);
	cv::imshow("Canny", imgCanny);
	cv::imshow("Dil", imgDil);

	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}


//Shapes Detection

//Нахождение объекта по цвету (HSV) - Color Detection
#if 0
int num_ph = 20;
std::string file_name = "./Test_photo/wt" + std::to_string(num_ph) + ".jpg";

//QR = cv::imread("./Test_photo/QR-Код.jpg", cv::IMREAD_GRAYSCALE);
image = imread(file_name, cv::IMREAD_COLOR);

cv::Mat imgHSV, mask;

cv::cvtColor(image, imgHSV, cv::COLOR_BGR2HSV);

cv::namedWindow("Trackbars", (640, 200));
cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);
cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);

cv::Scalar lower(hmin, smin, vmin);
cv::Scalar upper(hmax, smax, vmax);

cv::inRange(imgHSV, lower, upper, mask);

cv::imshow("origin", image);
//cv::imshow("Image HSV", imgHSV);
cv::imshow("Image mask", mask);

cv::waitKey(0);
/*
while (true)
{
	cv::Scalar lower(hmin, smin, vmin);
	cv::Scalar upper(hmax, smax, vmax);

	cv::inRange(imgHSV, lower, upper, mask);


	cv::imshow("origin", image);
	//cv::imshow("Image HSV", imgHSV);
	cv::imshow("Image mask", mask);

	cv::waitKey(1);

}*/
cv::destroyAllWindows();
#endif

//Определение объекта по шаблону с помощью "окна"
#if 0
QR = cv::imread("./Test_photo/QR-Код.jpg", cv::IMREAD_COLOR);

int num_ph = 12;
std::string file_name = "./Test_photo/wt" + std::to_string(num_ph) + ".jpg";

// получаем картинку
image = cv::imread(file_name, cv::IMREAD_COLOR);
cv::imshow("Before", image);

cv::Point p(0, 0);
//find_Object(image, QR, p, 0.5);
cv::matchTemplate(image, QR, dst, cv::TM_SQDIFF);
cv::minMaxLoc(dst, &minVal, &maxVal, &minLoc, &maxLoc);
Draw(image, minLoc, QR.cols, QR.rows);
cv::imshow("SQDIFF", image);
cv::matchTemplate(image, QR, dst, cv::TM_SQDIFF_NORMED);
cv::minMaxLoc(dst, &minVal, &maxVal, &minLoc, &maxLoc);
Draw(image, minLoc, QR.cols, QR.rows);
cv::imshow("SQDIFF_NORMED", image);
cv::matchTemplate(image, QR, dst, cv::TM_CCOEFF);
cv::minMaxLoc(dst, &minVal, &maxVal, &minLoc, &maxLoc);
Draw(image, maxLoc, QR.cols, QR.rows);
cv::imshow("CCOEFF", image);
cv::matchTemplate(image, QR, dst, cv::TM_CCOEFF_NORMED);
cv::minMaxLoc(dst, &minVal, &maxVal, &minLoc, &maxLoc);
Draw(image, maxLoc, QR.cols, QR.rows);
cv::imshow("CCOEFF_NORMED", image);
//Draw(image, p, QR.cols, QR.rows);

cv::waitKey(0);

cv::destroyAllWindows();
#endif

//Разбиение изображения на каналы
#if 0
int main()
{	
	QR = cv::imread("./Test_photo/QR-Код.jpg", cv::IMREAD_COLOR);

	int num_ph = 1;
	std::string file_name = "./Test_photo/wt" + std::to_string(num_ph) + ".jpg";

	// получаем картинку
	image = cv::imread(file_name, cv::IMREAD_COLOR);

	// покажем изображение
	cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
	cv::imshow("original", image);

	// The actual splitting.
	cv::split(image, channel);

	channel[1] = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC1);//Set green channel to 0
	//channel[0] = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC1);//Set blue channel to 0
	//channel[2] = cv::Mat::zeros(cv::Size(image.cols, image.rows), CV_8UC1);//Set red channel to 0

	 //Merging red and green channels

	cv::merge(channel, image);
	imshow("R+G", image);
	
	
	cv::waitKey(0);//Wait for a keystroke in the window

	return 0;
}
#endif

//Определение границ оператором Canny
#if 0
int main()
{
	int num_ph = 1;
	std::string file_name = "./Test_photo/wt" + std::to_string(num_ph) + ".jpg";

	// получаем картинку
	image = cv::imread(file_name, cv::IMREAD_COLOR);

	printf("[i] image: %s\n", file_name);
	

	// создаём одноканальные картинки
	cv::cvtColor(image, gray, cv::COLOR_RGB2GRAY);
	cv::cvtColor(image, dst, cv::COLOR_RGB2GRAY);
	cv::cvtColor(image, dst2, cv::COLOR_RGB2GRAY);
	//gray = cv::CreateImage(cv::GetSize(image), IPL_DEPTH_8U, 1);
	//dst = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);

	// окно для отображения картинки
	cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
	//cv::namedWindow("gray", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("cvCanny", cv::WINDOW_AUTOSIZE);
	cv::namedWindow("cvCanny2", cv::WINDOW_AUTOSIZE);

	// преобразуем в градации серого
	cv::cvtColor(image, gray, cv::COLOR_RGB2GRAY);
	

	// получаем границы
	cv::Canny(gray, dst2, 160, 550, 3);
	cv::Canny(gray, dst, 20, 100, 3);

	// показываем картинки
	cv::imshow("original", image);
	//cv::imshow("gray", gray);
	cv::imshow("cvCanny", dst);
	cv::imshow("cvCanny2", dst2);

	// ждём нажатия клавиши
	cv::waitKey(0);
	// удаляем окна
	cv::destroyAllWindows();
	return 0;
}
#endif
