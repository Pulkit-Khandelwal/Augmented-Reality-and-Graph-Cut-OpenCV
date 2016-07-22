// This code applys the grabcut algorithm on an image
#include <iostream>
#include <opencv/cv.h>
#include <math.h>
#include "opencv2/highgui/highgui.hpp"
#include <stdio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

// Variables Declaration explained later whenever they are encountered
int x1,x2,x3,x4,x5,x6;
int width, height;
Mat imageorg; Mat imagegray;
Mat threshimage;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
int j;
int a,b,c,d;
int lilsum1 = 1000;
int lilsum2 = 1000;
int maxCorners = 20;
void goodFeaturestToTrack_Demo( int, void* );// Function to detect the Corners
Mat dst_norm, dst_norm_scaled;
Mat dst = Mat::zeros( imageorg.size(), CV_32FC1 );
int blockSize = 2;
int apertureSize = 3;
double k = 0.04;
void thresh_callback(int, void* );

int main()
 {
	  imageorg = imread("framecap.jpg");// Takes an image on which garbcut is to applied
	  cvtColor( imageorg, imagegray, CV_BGR2GRAY );// Converts the RGB to Grayscale Image
	  blur(imagegray,imagegray, Size(3,3));// Blurs the Image
	  Mat canny_output;
	  vector<vector<Point> > contours;// Creates a set of Vectors to detect the contours
	  vector<Vec4i> hierarchy;// Create vectors which needs to be passed as a parameter to teh Grabcut function
	  threshold( imagegray, threshimage,30,255 ,THRESH_OTSU);// Image is thresholded
	  imwrite("threshedimg.jpg",threshimage);//Image saved as a jpeg file
	  findContours( threshimage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
	  // Find the contours in the Image
	  vector<vector<Point> > contours_poly( contours.size() ); // Vector points for the polygon contours
	  vector<Rect> boundRect( contours.size() );// vector a bounding rectangle
	  // Calculates the polygons and creates the contours
	  for( int i = 0; i < contours.size(); i++ )
	     	{  
	   		approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
	        	boundRect[i] = boundingRect( Mat(contours_poly[i]) );
	     	}
	
	   // Next, we keep all the rectangles which we need according to the contours' area and erase the unnecessary ones
	   // Keep the bounding rectangles between the areas of 15 and 50
	   double max_area=50;
	   for( int i = 0; i< contours.size(); i++ ) 
	 	{
	   		 double area=contourArea( contours[i],false);  
			 if(area>max_area)
			 contours.erase(contours.begin() + i);
	 	}
	    double min_area=15; // area threshold
	
	    for( int i = 0; i< contours.size(); i++ ) 
	 	{
			 double areab=contourArea( contours[i],false);  
			 if(areab<min_area)
	   		 contours.erase(contours.begin() + i);
		 }
	    // Mat declarations to create images
	    Mat drawing = Mat::zeros( threshimage.size(), CV_8UC3 );
	    Mat drawinggray = Mat::zeros( threshimage.size(), CV_8UC3 );
	    for( int i = 0; i< contours.size(); i++ )
		{
			   Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			   rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );// Creates the image with a
		       	   //given breath and length 
		           cvtColor( drawing,drawinggray, CV_BGR2GRAY );// Converts the above image to grayscale
		           // Check for the Corners
	       {
	         if( maxCorners < 1 ) 
	          	{
	        		 maxCorners = 1;
	        	 }
	   // Following are the declarations and assignmnets for the parameters for the gooFeaturesToTrack function
	   vector<Point2f> corners;
	   double qualityLevel = 0.01;
	   double minDistance = 10;
	   int blockSize = 3;
	   bool useHarrisDetector = false;
	   double k = 0.04;
	
	
	  Mat copy;
	  copy = drawing.clone();
	  // Harris Corner Detector is used to detect the corners
	  goodFeaturesToTrack( drawinggray,
	                       corners,
	                       maxCorners,
	                       qualityLevel,
	                       minDistance,
	                       Mat(),
	                       blockSize,
	                       useHarrisDetector,
	                       k );
	  
	  cout<<"** Number of corners detected: "<<corners.size()<<endl;
	  int r = 4;
	  for( int i = 0; i < corners.size(); i++ )
	     { circle( copy, corners[i], r, Scalar(rng.uniform(0,255), rng.uniform(0,255),
	                                                 rng.uniform(0,255)), -1, 8, 0 );
	    }
	
	  namedWindow( "Imagefinal", CV_WINDOW_AUTOSIZE );// Image Output in a Window
	  imshow( "Imagefinal", copy );// Image Output in a Window
	
	  Size winSize = Size( 5, 5 );
	  Size zeroZone = Size( -1, -1 );
	  TermCriteria criteria = TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );
	     
	     //cornerSubPix is used to determine the locations of the rectangle
	     cornerSubPix( drawinggray, corners, winSize, zeroZone, criteria );
	     
	     /* The following For loop is used to calculate the length and breadth used to determine the
	     rectangle. For the grabcut to be applied we need to have the (x,y) position of a rectangle along with
	     its length and height. The following For loop does the same*/
	    for( int i = 0; i < corners.size(); i++ )
		     { 
		      j=i+1;
		      a =  cvFloor(corners[i].y);
		      b =  cvFloor(corners[j].y);
		      c =  cvFloor(corners[i].x);
		      d =  cvFloor(corners[j].x);
		      x1 = cvFloor(sum(corners[i].y)[0]);
		      x2 = cvFloor(sum(corners[j].y)[0]);
		      x3 = cvFloor(sum(corners[i].x)[0]);
		      x4 = cvFloor(sum(corners[j].x)[0]);
		      
		      cout<<" -- Refined Corner ["<<i<<"]  ("<<corners[i].x<<","<<corners[i].y<<")"<<endl;
		     if(c == d)
		     {
		      height = abs(x1 - x2);
		     }
		     if(a == b)
		     {
		      width = abs(x4 - x3);
		     }
		     if(a < lilsum1)
		     {
		      lilsum1 = x1;
		     }
		     if(c < lilsum2)
		     {
		      lilsum2 = x3;
		     }
		       cout<< height<<endl;
		      cout<< width<<endl;
		      cout<<"x"<<lilsum2<<"y"<<lilsum1<<endl;        
		   }
	  }
	}
		cv::Rect rectangle(lilsum2,lilsum1,width,height);// The (x,y) position of the upper left corner of 
		// a rectangle along with its length and height. This is used as a parameter for the grabCut function
		//Variable declaration for the grabCut function
		cv::Mat result;
		cv::Mat bgModel,fgModel; 
		//grabCut function
		cv::grabCut(imageorg,    
		            result,   
		            rectangle,
		            bgModel,fgModel,
		            10,        
		            cv::GC_INIT_WITH_RECT); 
		
		cv::Mat foreground(imageorg.size(),CV_8UC3,cv::Scalar(255,255,255));
		imageorg.copyTo(foreground,result); 
		cv::rectangle(imageorg, rectangle, cv::Scalar(255,255,255),1);
		
		  //Original Image
		  namedWindow( "OriginalImage", CV_WINDOW_AUTOSIZE );
		  imshow("Original Image",imageorg);
		  
		  //Contours in the Image
		  namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
		  imshow( "Contours", drawing );
		
		  //Foreground having the segmented part with a white screen
		  namedWindow( "Foreground", CV_WINDOW_AUTOSIZE );
		  imshow("Foreground",foreground);      
		  
		  cvWaitKey(0);
		  return 0;
 }
