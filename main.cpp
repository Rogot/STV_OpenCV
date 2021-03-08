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

int main()
{
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
	return 0;
}

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
