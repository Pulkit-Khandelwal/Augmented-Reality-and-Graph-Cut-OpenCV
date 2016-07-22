#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
using namespace cv;
using namespace std;
int main()
{
VideoCapture stream1(0);
if(!stream1.isOpened())
{
cout <<"Cannot Open Camera";
}
while(true)
{
Mat frame;
stream1.read(frame);
imshow("cam", frame);
imwrite("frame.jpg",frame);
			}
			}			