#include <iostream>
#include <opencv/cv.h>
 #include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;
cv::Point2f center(0,0);
    cv::Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b)
{
    int x1 = a[0], y1 = a[1], x2 = a[2], y2 = a[3];
    int x3 = b[0], y3 = b[1], x4 = b[2], y4 = b[3];

    if (float d = ((float)(x1-x2) * (y3-y4)) - ((y1-y2) * (x3-x4)))
    {
        cv::Point2f pt;
        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;
        return pt;
    }
    else
        return cv::Point2f(-1, -1);
    
}

int main()
{
	Mat image;
	Mat rectgray;
	image = imread("rectimage.jpg");
	cvtColor(image, rectgray, CV_BGR2GRAY);
	cv::Canny(rectgray, rectgray, 100, 100, 3);
    namedWindow("Canny Edge Detector",CV_WINDOW_AUTOSIZE);
    imshow("Canny Edge Detector",rectgray);
	std::vector<cv::Vec4i> lines;
    HoughLinesP(rectgray, lines, 1, CV_PI/180, 70, 30, 10);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        line( rectgray, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
    }
    namedWindow("The detected lines",CV_WINDOW_AUTOSIZE);
    imshow("The detected lines",rectgray);


std::vector<cv::Point2f> corners;
for (int i = 0; i < lines.size(); i++)
{
    for (int j = i+1; j < lines.size(); j++)
    {
                cv::Point2f pt = computeIntersect(lines[i], lines[j]);
        if (pt.x >= 0 && pt.y >= 0)
            corners.push_back(pt);
                   }
       }

    cvWaitKey(0);
    return 0;
}