#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;

int main()
 {
    Mat frame;
    VideoCapture capture;//("01.png");
    capture.set(CV_CAP_PROP_POS_AVI_RATIO, 1);
    while(1)
    {
       VideoCapture capture("%02d.png"); 
             
        capture >> frame;
        capture.set(CV_CAP_PROP_POS_FRAMES, capture.get(CV_CAP_PROP_POS_FRAMES) - 2);
 
        imshow("frame",frame);
        if(waitKey(30)==27)
            break;
    }
    return 0;
} 
