#include <cv.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

int main( int argc, char** argv )
 {
   double alpha = 0.2; double beta; double input;
   VideoCapture stream1(0);
   beta = (1.0 - alpha);
   double dWidth = stream1.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
   double dHeight = stream1.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
   Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));
   VideoWriter oVideoWriter ("Augmented Video.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true);
   Mat src1 = imread("lady.jpg");
   while(true)
    {
      Mat frame;
      stream1.read(frame);
      Mat dst;

      //src2 = imread("frame.jpg");
      namedWindow("Linear Blend",1);
      addWeighted( src1, alpha, frame, beta, 0.0, dst);
      
      oVideoWriter.write(dst);
      imshow( "Linear Blend", dst );
      if (waitKey(20) >= 0)
            break;
     }
   return 0;
  }
