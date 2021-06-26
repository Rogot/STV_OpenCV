#include "Area.h"

namespace us
{
	Areas::Areas(std::vector<BGR>& ignoreColor)
	{
		this->ignoreColor = ignoreColor;
	}

	Areas::Areas()
	{

	}

	Areas::~Areas()
	{

	}

	Area::Area(std::string type)
	{
		this->type = type;	
	}
	Area::Area()
	{

	}
	Area::~Area()
	{

	}
}

namespace us
{
	void setColor(cv::Point3_<uchar>* curPix, int B, int G, int R)
	{
		curPix->x = B;
		curPix->y = G;
		curPix->z = R;
	}


	bool Areas::checkAreas(cv::Mat& input, cv::Point& p)
	{
		Point cur;
		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				cur.adrC = input.ptr<cv::Point3_<uchar> >(i, j);
				if ((cur.adrC->x && cur.adrC->y && cur.adrC->z) == 0)
				{
					p.x = j;
					p.y = i;
					return 1;
				}
			}
		}
		return 0;
	}

	void Areas::findColors(const cv::Mat& input)
	{
		cv::Mat temp;
		temp = input.clone();
		cv::Point3_<int>* p;
		us::BGR curbgr;

		for (int i = 0; i < input.rows; ++i)
		{
			for (int j = 0; j < input.cols; ++j)
			{
				p = temp.ptr<cv::Point3_<int> >(i, j);
				
				if (ignoreColor[0].B != p->x && ignoreColor[0].G != p->y && ignoreColor[0].R != p->z)
				{
					us::Area area;

					existingColors.push_back(area);
				}
			}
		}
	}

	void Areas::fillArea(const cv::Mat& input, cv::Mat& output, int startX, int startY)
	{
		us::Point left, up, down, cur, next;
		bool flag = false;
		int count = 0;

		int x = startX;
		int y = startY;

		output = input.clone();

		int B = 1 + rand() % 254;
		int G = 1 + rand() % 254;
		int R = 1 + rand() % 254;

		int c = 0;

		while (1)
		{
			c++;
			cur.adrC = output.ptr<cv::Point3_<uchar> >(y, x);
			up.adrC = output.ptr<cv::Point3_<uchar> >(y, x);
			down.adrC = output.ptr<cv::Point3_<uchar> >(y, x);
			left.adrC = output.ptr<cv::Point3_<uchar> >(y, x);
			next.adrC = output.ptr<cv::Point3_<uchar> >(y, x);
			if (y > 0) {
				up.adrC = output.ptr<cv::Point3_<uchar> >(y - 1, x);
				up.Y = y - 1;
				up.X = x;
			}
			if (y < output.rows - 1) {
				down.adrC = output.ptr<cv::Point3_<uchar> >(y + 1, x);
				down.Y = y + 1;
				down.X = x;
			}
			if (x > 0) {
				left.adrC = output.ptr<cv::Point3_<uchar> >(y, x - 1);
				left.Y = y;
				left.X = x - 1;
			}
			if (x < output.cols - 1) {
				next.adrC = output.ptr<cv::Point3_<uchar> >(y, x + 1);
				next.Y = y;
				next.X = x + 1;
			}
			if ((cur.adrC->x && cur.adrC->y && cur.adrC->z) != 255)
			{
				count++;
				if ((up.adrC->x == 0 && up.adrC->y == 0 && up.adrC->z == 0) && y > 0) {
					count = 0;
					setColor(cur.adrC, B, G, R);
					cur.adrC = up.adrC;
					y--;
				}
				else if ((next.adrC->x == 0 && next.adrC->y == 0 && next.adrC->z == 0) && x < output.cols - 1) {
					count = 0;
					setColor(cur.adrC, B, G, R);
					cur.adrC = next.adrC;
					x++;
				}
				else if ((down.adrC->x == 0 && down.adrC->y == 0 && down.adrC->z == 0) && y < output.rows - 1) {
					count = 0;
					setColor(cur.adrC, B, G, R);
					cur.adrC = down.adrC;
					y++;
				}
				else if ((left.adrC->x != 255 && left.adrC->y != 255 && left.adrC->z != 255) && x > 0)
				{
					count = 0;
					setColor(cur.adrC, B, G, R);
					cur.adrC = left.adrC;
					x--;
				}
				else if (up.adrC->x != 255 && up.adrC->y != 255 && up.adrC->z != 255)
				{
					setColor(cur.adrC, B, G, R);
					cur.adrC = up.adrC;
					if (y > 0) y--;
				}
				if (count > 200) break;
				if(c % 30 == 0){
					cv::imshow("imgCrop", output);
					cv::waitKey(0);
				}
			}
		}
	}

	int c = 0;

	void floodFill4(cv::Mat& input, int x, int y, us::BGR color)
	{
		us::Point cur;
		cv::Point p;
		cur.adrC = input.ptr<cv::Point3_<uchar> >(y, x);
		c++;
		if((cur.adrC->x != 255 && cur.adrC->y != 255 && cur.adrC->z != 255) && (cur.adrC->x != color.B && cur.adrC->y != color.G && cur.adrC->z != color.R))
		{
			setColor(cur.adrC, color.B, color.G, color.R);
			if (y > 0)
			floodFill4(input, x, y - 1, color);
			if (y < input.rows - 1)
			floodFill4(input, x, y + 1, color);
			if (x > 0)
			floodFill4(input, x - 1, y, color);
			if (x < input.cols - 1)
			floodFill4(input, x + 1, y, color);
		}
	}


	void Areas::FindAreas(const cv::Mat& input, cv::Mat& output)
	{

	}
}