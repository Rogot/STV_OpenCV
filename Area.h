#pragma once
#include <opencv2/opencv.hpp>
#include <string>

namespace us
{

	struct BGR {
		int B;
		int G;
		int R;
	};

	struct Point
	{
		cv::Point3_<uchar>* adrC;
		int X;
		int Y;
	};

	void floodFill4(cv::Mat& input, int x, int y, us::BGR color);

	class Area {
		std::vector<cv::Point> points;
		cv::Point centrOfArea;
		std::string type;

	public:
		Area();
		Area(std::string type);
		~Area();
		std::string getType() { return this->type; }
		void addPoints(cv::Point point) { this->points.push_back(point); }
		void FindCenter();
	};

	class Areas {
	private:
		std::vector<BGR> ignoreColor;
		std::vector<Area> existingColors;
	public:
		Areas(std::vector<BGR>& ignoreColor);
		Areas();
		~Areas();
		BGR getIgnoreCol(int index) { return this->ignoreColor[index]; }
		void addIgnoreCol(BGR color) { this->ignoreColor.push_back(color); }
		void findColors(const cv::Mat& input);
		void FindAreas(const cv::Mat& input, cv::Mat& output); 
		void fillArea(const cv::Mat& input, cv::Mat& output, int startX, int startY);
		bool checkAreas(cv::Mat& input, cv::Point& p);
	};
}