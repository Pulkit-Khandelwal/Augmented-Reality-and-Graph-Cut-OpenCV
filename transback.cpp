#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace cv;


int main()
{
  /*
	IplImage* image = cvLoadImage("yes.jpg");
	cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE);
    cvShowImage( "Example1",image);
    IplImage* mask = cvCreateImage(cvGetSize(image), 8, 3);
    */
   

   Mat image2 = imread("frame.jpg");// white+frog 640X480
   Mat image = imread("yes2.jpg"); //black+fruits 640X480
   cv::Mat image1(image.size(),CV_8UC3);
   cvtColor(image,image1,CV_BGR2HLS); 
   namedWindow("Display window", CV_WINDOW_AUTOSIZE);
   imshow("Display window",image);  
   cv::Mat mask(image.size(),CV_8U);

   inRange(image, 
           Scalar(0,0,0), 
           Scalar(0,0,0), 
           mask
           );
  //bitwise_not(mask,mask);
mask = 255 - mask;
    cv::Mat outimage(image1.size(),CV_8U);//cv::Scalar(0,0,0)
    image.copyTo(image2,mask);
    namedWindow("Output", CV_WINDOW_AUTOSIZE);
    imshow("Output",image2); 
    Mat imagecon;
   /* cvtColor(outimage,imagecon,CV_HLS2BGR);
    namedWindow("Output1", CV_WINDOW_AUTOSIZE);
    imshow("Output1",imagecon); */
    /*cvNot(mask, mask);
    IplImage* myImageWithTransparency;
cvCopy(image,myImageWithTransparency,mask);
cvNamedWindow( "Output", CV_WINDOW_AUTOSIZE);
    cvShowImage( "Output",myImageWithTransparency);*/

cvWaitKey(0);
return 0;
}