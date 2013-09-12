
#define PI 3.14159265
#include <iostream>
#include <fstream>
#include <string>
#include "conio.h"
#include <cv.h>
#include <cvaux.h>
#include <cxcore.h>
#include <highgui.h>

#include <stdio.h>
#include <stdlib.h>
#include "C:\\March01\\Thursday\\rt.h"			// change pathlist to this header file according to where it is stored


int main()
{
	while(1){


		CPipe pleft("leftMotorPipe",1024);
		double leftWheel;

		CPipe pright("rightMotorPipe",1024);
		double rightWheel;

		CPipe endpipe("EndPipe",1024);
		bool end;

		cout << "Enter Steps leftWheel is supposed to turn: \n";
		cin >> leftWheel;
		cout << "Enter Steps rightWheel is supposed to turn: \n";
		cin >> rightWheel;
		cout << "Enter 1 to exit the program: \n";
		cin >> end;

		pleft.Write(&leftWheel,sizeof(leftWheel));
		pright.Write(&rightWheel,sizeof(rightWheel));
		endpipe.Write(&end,sizeof(end));
		cout << "leftWheel is: " <<leftWheel << "\n";
		cout << "rightWheel is: " <<rightWheel << "\n";
		cout << "End is: " <<end<< "\n\n\n";

			//char c = cvWaitKey(1);
			//if(c==27){break;}

	}
	
	return 0 ;
}