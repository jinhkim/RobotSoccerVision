
#define PI 3.14159265
#include <iostream>
#include <fstream>
#include <string>
#include "conio.h"


#include <stdio.h>
#include <stdlib.h>
#include "rt.h"			// change pathlist to this header file according to where it is stored


int main()
{
	while(1){


		CPipe pleft("leftMotorPipe",1024);
		double leftWheel;

		CPipe pright("rightMotorPipe",1024);
		double rightWheel;


		cout << "Enter Steps leftWheel is supposed to turn: \n";
		cin >> leftWheel;   //RANDY: Feed these from GUI. 
		cout << "Enter Steps rightWheel is supposed to turn: \n";
		cin >> rightWheel;   // RANDY: Feed these from GUI. 


		// Enter left followed by right wheel motor steps

									
								//	ifstream inFile2;
							 //  
								//	inFile2.open("c:\\xcoord.txt");
								//	if (!inFile2) {
								//	cout << "Unable to open file";
								//	exit(1); // terminate with error
								//	}
							 //  
								//	while (inFile2 >> leftWheel) {
								//	cout << "Left Wheel is: " << leftWheel << endl;
								//	}
							 //  
								//	inFile2.close();
							 //  
								//	
								//	ifstream inFile3;
							 //  
								//	inFile3.open("c:\\ycoord.txt");
								//	if (!inFile3) {
								//	cout << "Unable to open file";
								//	exit(1); // terminate with error
								//	}
							 //  
								//	while (inFile3 >> rightWheel) {
								//	cout << "Right Wheel is: " << rightWheel << endl;
								//	}
							 //  
								//inFile3.close();





		pleft.Write(&leftWheel,sizeof(leftWheel));
		pright.Write(&rightWheel,sizeof(rightWheel));
		cout << "leftWheel is: " <<leftWheel << "\n";
		cout << "rightWheel is: " <<rightWheel << "\n\n\n";


			//char c = cvWaitKey(1);
			//if(c==27){break;}

	}
	
	return 0 ;
}