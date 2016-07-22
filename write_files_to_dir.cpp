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
		string filename;
		int cnt=0;
		if(!stream1.isOpened())
			{
				cout <<"Cannot Open Camera";
			}
		while(1)
			{
				Mat frame;
				stream1.read(frame);
				char file[10];
				cnt++;                               
				sprintf(file,"%d.png",cnt);      
				imwrite(file,frame);
    				char key = cvWaitKey(30);
        			if (key == 27) // ESC
            			break;
			}
		cout<<cnt;			
	}			
