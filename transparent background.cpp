#include <iostream>
#include <opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;


int main()
{
	IplImage* image = cvLoadImage("009.png");
	cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE);
    cvShowImage( "Example1",image);
    IplImage* amsk; 
    cvInRangeS(image, 
           cvScalar(125,0,0), 
           cvScalar(255, 127, 127), 
           mask
           );
    cvNot(mask, mask);
    IplImage *myImageWithTransparency;
cvCopy(myImage,myImageWithTransparency,mask);
cvNamedWindow( "Output", CV_WINDOW_AUTOSIZE);
    cvShowImage( "Output",myImageWithTransparency);
cvWaitKey();
return 0;
}