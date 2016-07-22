#include <iostream>
#include <opencv/cv.h>
 #include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;


Mat imageorg; Mat imagegray;
Mat drawinggray;
Mat threshimage;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
int x,y;
double area;

int main()
{
 imageorg = imread("framecap.jpg");
  cvtColor( imageorg, imagegray, CV_BGR2GRAY );
  blur(imagegray,imagegray, Size(3,3) );
  namedWindow( "Original Image", CV_WINDOW_AUTOSIZE );
  Mat canny_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  threshold( imagegray, threshimage,70,255 ,THRESH_OTSU);
findContours( threshimage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );


  for( int i = 0; i < contours.size(); i++ )
     { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
       boundRect[i] = boundingRect( Mat(contours_poly[i]) );
  }
 for( int i = 0; i< contours.size(); i++ ) // iterate through each contour.
 {
   double area=contourArea( contours[i],false);  //  Find the area of contour
   if(area>max_area)
    contours.erase(contours.begin() + i);
 }

Mat drawing = Mat::zeros( threshimage.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       //drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
      // drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
}

 namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  imshow( "Contours", drawing );
 imwrite("Contours.jpg",drawing);
waitKey();
return 0;
}




