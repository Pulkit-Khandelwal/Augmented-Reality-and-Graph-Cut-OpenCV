#include <cv.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

int main()
 {

  VideoCapture stream1(0);//camera input
  VideoCapture stream2("Black2.mp4");//video file to be overlayed

  Mat frame1;//frame for stream1
  Mat frame2;//frame for stream2

  //Video Writer:
  double dWidth = stream1.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
  double dHeight = stream1.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video
  Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));//frame size
  VideoWriter oVideoWriter ("Blended.avi", CV_FOURCC('P','I','M','1'), 20, frameSize, true);//Video write
  //Infinite Loop for raeding frames:
  while(true)
   {
    stream1.read(frame1);//read the frame of camera
    stream2.read(frame2);//read the frame of the video
    cv::Mat mask(frame2.size(),CV_8U);//matrix initialization and this needs to be of the same size as the frames
    //The following function is to remove the black part in the video:
    //The video frame is taken and the black part is recognized using the range specified in the second and third 
    //parameter and a mask is created which is stored as mask 
    inRange(frame2, 
           Scalar(0,0,0), 
           Scalar(0,0,0), 
           mask
           );
    mask = 255 - mask;// subtract the above black mask created from the image
    frame2.copyTo(frame1,mask);//copy the video frame with mask(i.e. black part removed) onto the camera feed
    namedWindow("Linear Blend");//display the final output
    imshow( "Linear Blend",frame1);//Video overlay on the camera feed
    oVideoWriter.write(frame1);//for saving the video
    if (waitKey(20) >= 0)
            break;
   }
 return 0;
   
 }
