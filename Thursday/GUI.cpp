
// GUI.cpp : Defines the entry point for the console application.
//

// GUI.cpp : Defines the entry point for the console application.
// Created for build/install tutorial, Microsoft Visual C++ 2010 Express and OpenCV 2.1.0

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

int main(int argc, char* argv[])
{
	int trackbalVal=1;
	int maxVal=1;
	int pos = cvGetTrackbarPos("GUI","GUI");

	cvNamedWindow("GUI", CV_WINDOW_AUTOSIZE); 

	cvCreateTrackbar("GUI","GUI",&trackbalVal,maxVal, NULL );
	cvSetTrackbarPos("GUI","GUI", 0);

	//CvCapture* capture=cvCaptureFromCAM(0);
	CvCapture* capture = cvCaptureFromFile("http://137.82.120.10:8008.mjpeg");

	//loop and keep displaying web cam image

	//printf("Press ESC to exit GUI.\n\n");
	//printf("Press ENTER to output coordinates.\n\n");
	a:while( 1 ) {

	//break out of loop when enter key pressed (27 is esc)
	if (cvWaitKey(100)== 13) break;
	//exit when esc key pressed

	//get's image
	IplImage* frame = cvQueryFrame(capture);
	//display's image in the window
	
	//if (cvGetTrackbarPos("GUI","GUI") == 1)
	//{
	//cvDestroyWindow("GUI");
	//system("CLS");
	//goto a;
	//}

	cvShowImage("GUI", frame);
	}

	//clean up and destroy Demo window
	cvDestroyWindow("GUI");
	system("CLS"); 

	//open .exe
	(void)system("thursday.exe");
	system("CLS");
	printf("GUI is online.\n");
	//if(getchar() == 27) exit(0);
	goto a;

	return 0;
}





