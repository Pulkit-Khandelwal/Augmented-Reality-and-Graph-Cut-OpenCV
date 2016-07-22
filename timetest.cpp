#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;


#define DECLARE_TIMING(s)	int64 timeStart_##s; int64 timeDiff_##s; int64 timeTally_##s = 0; int64 countTally_##s = 0
#define START_TIMING(s)		timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s)		timeDiff_##s = (cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s)		(double)(0.001 * ( (double)timeDiff_##s / (double)cvGetTickFrequency() ))
#define GET_AVERAGE_TIMING(s)	(double)(countTally_##s ? 0.001 * ( (double)timeTally_##s / ((double)countTally_##s * cvGetTickFrequency()) ) : 0)
#define GET_TIMING_COUNT(s)	(int)(countTally_##s)
#define CLEAR_AVERAGE_TIMING(s)	timeTally_##s = 0; countTally_##s = 0
#define SHOW_TIMING(s, msg)	printf("%s time: \t %dms \t (%dms average across %d runs).\n", msg, cvRound(GET_TIMING(s)), cvRound(GET_AVERAGE_TIMING(s)), GET_TIMING_COUNT(s) )


int main()
{
	DECLARE_TIMING(read_frame);
	DECLARE_TIMING(camera_open);
	DECLARE_TIMING(display_frame);
	DECLARE_TIMING(write_frame);    

START_TIMING(camera_open);    
VideoCapture stream1(0);
STOP_TIMING(camera_open);

if(!stream1.isOpened())
{
cout <<"Cannot Open Camera";
}
while(true)
{
Mat frame;
START_TIMING(read_frame);
stream1.read(frame);
STOP_TIMING(read_frame);
START_TIMING(display_frame);
imshow("cam", frame);
STOP_TIMING(display_frame);
START_TIMING(write_frame);
imwrite("frame.jpg",frame);
STOP_TIMING(write_frame);

    SHOW_TIMING(camera_open, "Open the camera");
    SHOW_TIMING(read_frame, "Read a frame");	
	SHOW_TIMING(display_frame, "Display the frame");
	SHOW_TIMING(write_frame, "Save a frame");
			}
			}
			



