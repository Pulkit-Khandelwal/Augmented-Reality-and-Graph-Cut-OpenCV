//This code detects a person or people using HoG Descriptors and saves it as a video with a black screen

//Libraries called along with packages
#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

using namespace std;
using namespace cv;

//The following lines are used to define stuff related to calculate the time elapsed for a code fragment
// Can be ignored for mobile device

#define DECLARE_TIMING(s)   int64 timeStart_##s; int64 timeDiff_##s; int64 timeTally_##s = 0; int64 countTally_##s = 0
#define START_TIMING(s)     timeStart_##s = cvGetTickCount()
#define STOP_TIMING(s)      timeDiff_##s = (cvGetTickCount() - timeStart_##s); timeTally_##s += timeDiff_##s; countTally_##s++
#define GET_TIMING(s)       (double)(0.001 * ( (double)timeDiff_##s / (double)cvGetTickFrequency() ))
#define GET_AVERAGE_TIMING(s)   (double)(countTally_##s ? 0.001 * ( (double)timeTally_##s / ((double)countTally_##s * cvGetTickFrequency()) ) : 0)
#define GET_TIMING_COUNT(s) (int)(countTally_##s)
#define CLEAR_AVERAGE_TIMING(s) timeTally_##s = 0; countTally_##s = 0
#define SHOW_TIMING(s, msg) printf("%s time: \t %dms \t (%dms average across %d runs).\n", msg, cvRound(GET_TIMING(s)), cvRound(GET_AVERAGE_TIMING(s)), GET_TIMING_COUNT(s) )
 // The Main Function
int main (int argc, const char * argv[])
{
    /* These declarations are for the calculation of time elapsed for a code fragment
    So, commands like :
    DECLARE_TIMING();
    START_TIMING();
    STOP_TIMING(); 
    can be ignored*/
    DECLARE_TIMING(video_open);
    DECLARE_TIMING(algorithm);
    DECLARE_TIMING(write_video); 
    DECLARE_TIMING(read_frame); 
    Mat dst; // Variable to store the final output frame

    START_TIMING(video_open); 
    VideoCapture cap("person1.mp4");// Creates object for the video feed
    STOP_TIMING(video_open);

    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);// Gets frame width
    double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);// Gets the frame height
    Size frameSize(static_cast<int>(dWidth), static_cast<int>(dHeight));// Defines the Frame Size
    VideoWriter oVideoWriter ("Person_detect2.avi", CV_FOURCC('P','I','M','1'),30, frameSize, true);
    //Creates an object for the recording of the video as it has been stored as a file(any format like: avi,mp4) etc

  

    if (!cap.isOpened())// Checks if there is a camera feed or not
        return -1;
 int cnt=0;
   Mat img;
   Mat frame;

    HOGDescriptor hog; // Creates a variable for HoG Descriptor
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    // A Support Vectore Machine (SVM) for HoG Descriptor. This is for people detection

    while (true)// Infinite Loop for Reading Frames for the length of the video
    {
        
    START_TIMING(read_frame);
    cap >> img;// Reads a frame from the camera
    STOP_TIMING(read_frame);
            if (!img.data)
            continue; 

        START_TIMING(algorithm); 

        vector<Rect> found, found_filtered;// Vectors for HoG
        hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);//Parameters of HoG
        // The following loop checks for a person using HoG and alongwith that GrabCut is applied to
        //remove the part we are interested in
        size_t i, j;
        for (i=0; i<found.size(); i++)
        {
            Rect r = found[i];
            for (j=0; j<found.size(); j++)
                if (j!=i && (r & found[j])==r)
                    break;
            if (j==found.size())
                found_filtered.push_back(r);
        }
        for (i=0; i<found_filtered.size(); i++)
        {
        Rect r = found_filtered[i];
        r.x += cvRound(r.width*0.1);
        r.width = cvRound(r.width*0.8);
        r.y += cvRound(r.height*0.06);
        r.height = cvRound(r.height*0.9);
        rectangle(img, r.tl(), r.br(), cv::Scalar(0,0,0), 2);
        cv::Mat result;
        cv::Mat bgModel,fgModel; 
        cv::grabCut(img,    
            result,   
            r,
            bgModel,fgModel,
            10,        
            cv::GC_INIT_WITH_RECT); 

cv::Mat foreground(img.size(),CV_8UC3,cv::Scalar(0,0,0));
// A variable foreground is declared which stores a black background
img.copyTo(foreground,result); 
//The orginial image i.e. a frame is copied onto the black background with the grabcut applied.
//So, we get our final result where there is a person on a black background

STOP_TIMING(algorithm); 

namedWindow( "Foreground", CV_WINDOW_AUTOSIZE );//Creates a Window
imshow("Foreground",foreground);//Shows the result

    START_TIMING(write_video); 
    oVideoWriter.write(foreground);// Writes the final result and saves it as a file
    STOP_TIMING(write_video); 
   
}                   
    SHOW_TIMING(video_open, "Open the video");
    SHOW_TIMING(write_video, "Write the video");    
    SHOW_TIMING(algorithm, "The Algorithm");
    SHOW_TIMING(read_frame, "Read the frame");
    waitKey(600);
    
    // When the entire video is read the code is terminated
    if(img.empty())
    {
            break;
        }
    }
      return 0; 
} 