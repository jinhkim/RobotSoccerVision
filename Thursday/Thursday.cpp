#include <afxcoll.h>
#include <windows.h>
#include "Math.h"
#include "HeadingRange.h"
#include "Line2D.h"
#include "Obstacle.h"
#include "Point2D.h"
#include "Rover.h"
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
#include "rt.h"			// change pathlist to this header file according to where it is stored
#include "Serial.h"

using namespace std;

//Struct for ST400NT motor control data
struct MotorData{
	double leftWheel;
	double rightWheel;
	int stop;
};

struct keyboarddata{
	double leftWheelRevs;
	double rightWheelRevs;
};

struct MotorData ST400;
struct keyboarddata data;

struct botData{
	double	posXbot,
		posYbot,
		g_angle,
		posXball,
		posYball;
	vector<vector<double>> balls;
	vector<vector<double>> obstacles;
};

void sensorFeedback(CPipe motorDone, CPipe motor, MotorData ST400, CSerial serial){


	while(motorDone.TestForData() == 0){
		//Check if a sensor detects something, otherwise wait until
		//motor indicates it has completed current instructions

		/*Insert sensor logic here (maybe a separate thread or new class?)*/
		//Make sure ST400.stop is set to 1 if a sensor interrupt occurs
		//and write to ST400Control

		if (serial.Open(4, 9600))
		{
			char* lpBuffer  = new char[100];
			int nBytesRead = serial.ReadData(lpBuffer, 100);
			strtok(lpBuffer,"\n");


			if(atoi(lpBuffer) == 1){
				ST400.stop=1;
				cout<<"HIT"<<endl;
				//cout<<"YAY I READ FROM ARDRUINO!"<<endl;
				motor.Write(&ST400, sizeof(ST400));
				ST400.stop=0;
				ST400.leftWheel=0; //Prevent rover from executing remaining steps
				ST400.rightWheel=0;
				break;
			}
			else{
				ST400.stop=0;
				cout<<"MISS"<<endl;
			}


		}

	}

};

int main (){

	CProcess p1("C:\\March01\\Debug\\ColourDetection.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
		);

	CProcess p3("C:\\March01\\Debug\\ST400Control.exe",	// pathlist to child program executable				
		NORMAL_PRIORITY_CLASS,			// priority
		OWN_WINDOW,						// process has its own window					
		ACTIVE							// process is active immediately
		);

	double debugging;
	double obstacleSensor = 0;

	double steps = 400;  // Set to 400 half-steps per Revolution.

	string sensorVoltage;
	double sensorVolt;

	struct botData roverData; // For Reading data from Image Processor.
	roverData.posXbot = 0.0;
	roverData.posYbot = 0.0;
	roverData.g_angle = 0.0;

	CSerial serial; // CREATE A SERIAL Object

	if (serial.Open(8, 9600))
		printf("Port opened successfully");
	else
		printf("Failed to open port!");

	cout << "Please Enter Debugging Mode \n";
	cout << "0 - Send A Number of Steps to Left and Right Wheels. \n";
	cout << "1 - Rover Travels to a X-Y Coordinate we type in. \n";
	cout << "2 - Rover Follows One Ball autonomously. \n";
	cout << "3 - Find all balls autonomously. \n";
	cin >> debugging;

	Rover rover1; 

	if(debugging==0){

		cout << "Entered Debugging Mode 0: Enter Left followed by Right Wheel Motor Steps \n";

		CProcess p("C:\\March01\\Debug\\KeyboardInput.exe",	// pathlist to child program executable				
			NORMAL_PRIORITY_CLASS,			// priority
			OWN_WINDOW,						// process has its own window					
			ACTIVE							// process is active immediately
			);



		cout << "\n Right after Process created";

		CPipe pleft("leftMotorPipe",1024);
		CPipe pright("rightMotorPipe",1024);

		cout << "\n After pleft and pright pipes created\n\n";

		double leftWheelRead=0;
		double rightWheelRead=0;
		double leftFirst=0;
		double rightFirst=0;

		ST400.leftWheel=0;
		ST400.rightWheel=0;
		ST400.stop = 0;

		CPipe motor("MotorData",1024);		// WRITE-ONLY pipeline to ST400Control
		CPipe motorDone("Stopped", 1024);	// READ-ONLY pipeline from ST400Control
		int status = 0;						// =1 if robot motion is complete 


		while(true){


			//Check if the user has input values for both the left and right wheels and read the data if they have.
			if(pleft.TestForData()>=sizeof(leftWheelRead)  &&  pright.TestForData()>=sizeof(rightWheelRead))
			{
				pleft.Read(&leftWheelRead,sizeof(leftWheelRead));
				cout << "leftWheel is: " <<leftWheelRead << "\n";
				pright.Read(&rightWheelRead,sizeof(rightWheelRead));
				cout << "rightWheel is: " <<rightWheelRead << "\n";
			}//



			//Nested while loop - only executes when there are steps to be travelled
			while(leftWheelRead!=0 && rightWheelRead!=0)
			{

				//If the steps are not equal for each motor, turn rover
				if(abs(leftWheelRead) != abs(rightWheelRead)){


					//Turn Left
					if(leftWheelRead>rightWheelRead)
					{
						leftFirst = (leftWheelRead - rightWheelRead)/2.0;
						rightFirst = -leftFirst;
						leftWheelRead = rightWheelRead;
						printf("Turning left...\n");
					}

					//Turn right
					else{
						rightFirst = (rightWheelRead - leftWheelRead)/2.0;
						leftFirst = -rightFirst;
						rightWheelRead = leftWheelRead;
						printf("Turning right...\n");
					}

					ST400.leftWheel = (leftFirst<5000) ? leftFirst : 5000;
					ST400.rightWheel = (rightFirst<5000) ? rightFirst : 5000;

					//Send the instructions to ST400Control
					motor.Write(&ST400, sizeof(ST400));

					//sensorFeedback(motorDone, motor, ST400, serial); // THE WHILE LOOP REPLACED HERE. 
					motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
					if(status==1)
					{
						printf("Motion completed...\n");
					}

				}
				//Otherwise move straight ahead
				else{
					ST400.rightWheel= (rightWheelRead < 5000) ? rightWheelRead : 5000;
					ST400.leftWheel= (leftWheelRead < 5000) ? leftWheelRead : 5000;
					leftWheelRead=0;
					rightWheelRead=0;
					motor.Write(&ST400,sizeof(ST400));
					printf("Driving straight ahead...\n");

					//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
					motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
					if(status==1)
					{
						printf("Motion completed...\n");
					}
				}

			}
			char cc = cvWaitKey(1);
			if(cc == 27) break;
		}
	} // End of Debugging 0.


	else if (debugging == 1){



		//double xInput;
		//ifstream inFile;
		//  
		//inFile.open("c:\\xcoord.txt");
		//if (!inFile) {
		//	cout << "Unable to open file";
		//	exit(1); // terminate with error
		//}
		//  
		//while (inFile >> xInput) {
		//	cout << "X coordinate is: " << xInput << endl;
		//}
		//  
		//inFile.close();
		//  
		//double yInput;
		//ifstream inFile1;
		//  
		//inFile1.open("c:\\ycoord.txt");
		//if (!inFile1) {
		//	cout << "Unable to open file";
		//	exit(1); // terminate with error
		//}
		//  
		//while (inFile1 >> yInput) {
		//	cout << "Y coordinate is: " << yInput << endl;
		//}
		//  
		//inFile1.close();

		CPipe motor("MotorData",1024);		//WRITE-ONLY pipeline to ST400Control
		CPipe motorDone("Stopped",1024);	//READ-ONLY pipeline from ST400Control
		ST400.leftWheel=0;
		ST400.rightWheel=0;
		ST400.stop=0;

		int status = 0;						//=1 if robot motion is complete

		CPipe pMainPipe("MainPipe",1024);

		while(true){

			cout << "Entered Debugging Mode 1: Enter Coordinates for Rover to Go to. \n" ;
			double xInput, yInput;
			cout << "Please Enter Destination X Coordinate: [0 to 720] \n" ; 
			cin >> xInput;
			cout << "Please Enter Destination Y Coordinate: [0 to 480] \n" ;
			cin >> yInput;

			//char c = cvWaitKey(1);
			//if(c == 27) break;

			while(roverData.posXbot < 170 || roverData.posXbot > 570 
				|| roverData.posYbot < 2 || roverData.posYbot > 474){
					pMainPipe.Read(&roverData,sizeof(roverData));
			}

			rover1.coordinate.x = roverData.posXbot;  // RANDY: Display rover x coordinate from this variable.
			rover1.coordinate.y = roverData.posYbot;  // RANDY: Display rover y coordinate from this variable.
			rover1.direction = roverData.g_angle;   // RANDY: Display rover angle from this variable. 

			Point2D ball(xInput, yInput);
			double ballxUpper = ball.x+30;
			double ballxLower = ball.x-30;
			double ballyUpper = ball.y+30;
			double ballyLower = ball.y-30;

			if((rover1.coordinate.x < ballxUpper) 
				&& (rover1.coordinate.x > ballxLower)
				&& (rover1.coordinate.y < ballyUpper)
				&& (rover1.coordinate.y > ballyLower)){
					//return 0;
			}
			// Calculate Heading.
			rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
			double headingInRevolutions  = rover1.headingRange.heading*980/360;// * (1/(10*PI))*((16*PI)/360);  // RANDY: Display headingInRevolutions and rename to turning steps.

			// Calculate Range.
			double rangeInRevolutions = rover1.headingRange.range*6.04;// * 0.508/(10*PI); // RANDY: Display rangeinRevolutions and rename to range steps. 

			/* Motor control starts here ************************************************************************/

			if(headingInRevolutions < -50){   // If ClockWise. LEFTWHEEL = MORE.
				//				double turnRate;
				ST400.leftWheel =  headingInRevolutions;
				ST400.rightWheel = -headingInRevolutions;


				//ST400.leftWheel = ST400.leftWheel/2.0;
				//ST400.rightWheel = -ST400.leftWheel;

				motor.Write(&ST400, sizeof(ST400));
				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE.	
				motorDone.Read(&status,sizeof(status));		//Empty pipe to prevent infinite loop

				while(motorDone.TestForData() == 0){  // Wait until done. - Added.
				}
			}


			else if(headingInRevolutions > 50){   // If CounterClockWise. RIGHTWHEEL = MORE.
				//ST400.leftWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
				//ST400.rightWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;

				ST400.leftWheel =  -headingInRevolutions;
				ST400.rightWheel =  headingInRevolutions;

				motor.Write(&ST400, sizeof(ST400));
				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.
				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop

				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
				}
			}

			//if(headingInRevolutions==0){  // If Straight.
			ST400.leftWheel =  rangeInRevolutions;
			ST400.rightWheel =  rangeInRevolutions;


			if(motor.TestForData() == 0 && ST400.rightWheel!=0){
				motor.Write(&ST400,sizeof(ST400)); 
				printf("Going Straight: wrote identical data to both wheels\n");
				//Reset these commands to 0 after sending the instruction
				//so the motors run for the input steps only once
				ST400.leftWheel=0;
				ST400.rightWheel=0;

				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
				}

			}


			if(motor.TestForData()==0)
			{
				printf("Pipe empty\n");
			}

			//char cc = cvWaitKey(1);
			//if(cc == 27) break;

			//} // End of While Loop.

			//serial.Close();

		}
	}// End of Debugging Mode 1.


	else if(debugging==2){

		cout << "Entered Debugging Mode 2: Follow One Ball Autonomously \n" ;

		CPipe motor("MotorData",1024);		//WRITE-ONLY pipeline to ST400Control
		CPipe motorDone("Stopped",1024);	//READ-ONLY pipeline from ST400Control
		ST400.leftWheel=0;
		ST400.rightWheel=0;
		ST400.stop=0;

		int status = 0;						//=1 if robot motion is complete

		while(true){

			char c = cvWaitKey(1);
			if(c == 27)break;

			CPipe pMainPipe("MainPipe",1024);

			pMainPipe.Read(&roverData,sizeof(roverData));

			rover1.coordinate.x = roverData.posXbot;    // RANDY: Here are Rover X coordinates to be displayed
			rover1.coordinate.y = roverData.posYbot;	// RANDY: Here are Rover Y coordinates to be displayed
			rover1.direction = roverData.g_angle;		// RANDY: Here is the rover's angle to be displayed

			if(roverData.posYball == 0 || roverData.posYball == 0){
				cout<<"Cant find anymore balls"<<endl;
				break;
			} else {

				double ballx = roverData.posXball;		// RANDY: x and y coordinate of ball is stored in these variables
				double bally = roverData.posYball;

				Point2D ball(ballx, bally);


				//double obstacleCoord[1][1];
				//obstacleCoord[0][0]=roverData.obstacles[0][0];
				//Obstacle obstacle[1][1];
				//Point2D temp(obstacleCoord[0][0], obstacleCoord[0][1]);
				//obstacle[0].obstacleCoordinate=temp;
				//obstacle[0].radius = 10; 
				//
				//Line2D destinationPath = Line2D(rover1.coordinate, ball);

				//if(rover1.checkIntersect(obstacle[minObstacleIndex], path[min_index])){
				//	rover1.headingRange=rover1.headingRange=rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
				//	rover1.headingRange=rover1.obstacleAvoidance(obstacle[0], ball);
				//}

				//else{
				//	rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
				//}

				// Calculate Heading.
			rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
			double headingInRevolutions  = rover1.headingRange.heading*980/360;// * (1/(10*PI))*((16*PI)/360);  // RANDY: Display headingInRevolutions and rename to turning steps.

			// Calculate Range.
			double rangeInRevolutions = rover1.headingRange.range*6.04;// * 0.508/(10*PI); // RANDY: Display rangeinRevolutions and rename to range steps. 

			/* Motor control starts here ************************************************************************/

			if(headingInRevolutions < -50){   // If ClockWise. LEFTWHEEL = MORE.
				//				double turnRate;
				ST400.leftWheel =  headingInRevolutions;
				ST400.rightWheel = -headingInRevolutions;


				//ST400.leftWheel = ST400.leftWheel/2.0;
				//ST400.rightWheel = -ST400.leftWheel;

				motor.Write(&ST400, sizeof(ST400));
				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE.	
				motorDone.Read(&status,sizeof(status));		//Empty pipe to prevent infinite loop

				while(motorDone.TestForData() == 0){  // Wait until done. - Added.
				}
			}


			else if(headingInRevolutions > 50){   // If CounterClockWise. RIGHTWHEEL = MORE.
				//ST400.leftWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
				//ST400.rightWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;

				ST400.leftWheel =  -headingInRevolutions;
				ST400.rightWheel =  headingInRevolutions;

				motor.Write(&ST400, sizeof(ST400));
				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.
				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop

				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
				}
			}

			//if(headingInRevolutions==0){  // If Straight.
			ST400.leftWheel =  rangeInRevolutions - 50;
			ST400.rightWheel =  rangeInRevolutions - 50;


			if(motor.TestForData() == 0 && ST400.rightWheel!=0){
				motor.Write(&ST400,sizeof(ST400)); 
				printf("Going Straight: wrote identical data to both wheels\n");
				//Reset these commands to 0 after sending the instruction
				//so the motors run for the input steps only once
				ST400.leftWheel=0;
				ST400.rightWheel=0;

				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
				}

			}




				if(motor.TestForData()==0)
				{
					printf("Pipe empty\n");
				}

				char cc = cvWaitKey(1);
				if(cc == 27) break;
			}

		} // End of While Loop.

		serial.Close();

	} // End of Debugging Mode 2.




	else if(debugging == 3){  // Start Debugging = 3

		cout << "Entered Debugging Mode 3: Track all 6 balls Autonomously \n" ;
		CPipe pMainPipe("MainPipe",1024);   // Image Processing Pipe
		CPipe motor("MotorData",1024);		// WRITE-ONLY pipeline to ST400Control
		CPipe motorDone("Stopped", 1024);	// READ-ONLY pipeline from ST400Control
		ST400.leftWheel=0;
		ST400.rightWheel=0;
		ST400.stop = 0;

		int status = 0;						// =1 if robot motion is complete 

		rover1.taskStatus=0; // keeps track of whether rover is pursuing ball, or pursuing endline.
		// 0 = Pursuing Ball. 1 = Has ball already, going to Endline. 
		// After reading Endling, rover1.taskStatus is reset to 0.

		while(1){
			char c = cvWaitKey(1);
			if(c == 27)break;

			pMainPipe.Read(&roverData,sizeof(roverData));

			rover1.coordinate.x = roverData.posXbot;  // RANDY: rover x 
			rover1.coordinate.y = roverData.posYbot;  // RANDY: rover y
			rover1.direction = roverData.g_angle;  // RANDY: rover angle.

			double ballCoord[6][2];  
			for(int k=0;k<6;k++){
				for(int m=0;m<2;m++){
					ballCoord[k][m]=roverData.balls[k][m];
				}
			}	

			Point2D ball[6];
			for(int i=0;i<6;i++){
				ball[i]=Point2D(ballCoord[i][0], ballCoord[i][1]);
				cout << "Ball "<<i<<" created with coordinates "<<ball[i].x<< " , "<<ball[i].y<<"\n";
				// RANDY: Display ALL items in ball coordinate array. ie same fashion as couts above.
			}

			double obstacleCoord[6][2];
			for(int k=0;k<6;k++){
				for(int m=0;m<2;m++){
					obstacleCoord[k][m]=roverData.balls[k][m];
				}
			}	

			Obstacle obstacle[6];
			for(int i=0;i<6;i++){
				Point2D temp(obstacleCoord[i][0], obstacleCoord[i][1]);
				obstacle[i].obstacleCoordinate=temp;
				obstacle[i].radius = 5; // radius of bottle. Supply me with this also
				cout << "Obstacle "<<i<<" created with coordinates "<<obstacle[i].obstacleCoordinate.x<< " , "<<obstacle[i].obstacleCoordinate.y<<"\n";

				//RANDY: Display ALL items in obstacle coordinates array. ie same fashion as above.
			}

			if(rover1.taskStatus == 0){   // fetch ball
				while(true){
					char c = cvWaitKey(1);
					if(c == 27)break;

					if (serial.Open(4, 9600))
					{
						char* lpBuffer  = new char[100];
						int nBytesRead = serial.ReadData(lpBuffer, 100);
						strtok(lpBuffer,"\n");

						//cout<<lpBuffer<<endl;
						if(1){
							//lpBuffer++;
							//getline((string)lpBuffer,sensorVoltage);
							//cout<<lpBuffer<<"V"<<endl;


							if(atoi(lpBuffer) == 1){
								obstacleSensor=1;
								cout<<"HIT"<<endl;
								//cout<<"YAY I READ FROM ARDRUINO!"<<endl;
							}
							else{
								obstacleSensor=0;
								cout<<"MISS"<<endl;
							}
						}

						delete [] lpBuffer;
						Sleep(150);
					}
				}

				if(obstacleSensor == 1){   // FROM THE ARDRUINO SENSORS = THERE IS AN OBSTACLE AHEAD.

					double rAngle = rover1.direction;
					double xIncremental;
					double yIncremental;

					if(rAngle>0 && rAngle <90){
						double tempo = tan(rAngle);
						yIncremental = tempo;
						xIncremental = 1;
					}
					else if(rAngle>90 && rAngle <180){
						double tempo = tan(180 - rAngle);
						yIncremental = tempo;
						xIncremental = -1;
					}
					else if(rAngle>180 && rAngle <270){
						double tempo = tan(270 - rAngle);
						yIncremental = -tempo;
						xIncremental = -1;
					}
					else{
						double tempo = tan(360 - rAngle);
						yIncremental = -tempo;
						xIncremental = 1;
					}

					Point2D destination1 = Point2D(rover1.coordinate.x+xIncremental*20, rover1.coordinate.y+yIncremental*20);
					Line2D destinationPath = Line2D(rover1.coordinate, destination1);

					rover1.ballStatus=1;
					for(int i=0; i<5; i++){
						if(rover1.checkIntersect(obstacle[i],destinationPath)){
							rover1.ballStatus=0;   // Not a Ball, it's an Obstacle.
						}
					}


					if(rover1.ballStatus==1){
						rover1.taskStatus=1;   // If it is a ball, then fetch ball set taskStatus to pursue endLine.
					}

					//Sending Data to Ardruino.

					if (serial.Open(4, 9600))   // port number (1~4) and baud rate   //Open  Serial Port:
					{
						const char* szMessage = (const char*)rover1.taskStatus;    // type input here
						int nBytesSent = serial.SendData(szMessage, strlen(szMessage));
					}
				}

				else{	

					rover1.taskStatus=0;  
					//Make 6 Paths
					Line2D path[6];
					for(int i=0;i<6;i++){
						path[i] = Line2D(rover1.coordinate, ball[i]);
						cout << "Path "<<i<<" created with distance "<< rover1.calcRange(rover1.coordinate, ball[i])<<" \n";
					}

					//Min Ball
					double min = path[0].range();
					int min_index = 0;
					for(int i=0; i<6; i++){
						if(path[i].range()<min){
							min = path[i].range();
							min_index=i;
						}
					}
					cout << "Min Ball Is " << ball[min_index].x << "," << ball[min_index].y<<"\n";

					//Ball[min_index] to Obstacle Distances
					Line2D ballLine[6];
					for(int i=0;i<6;i++){
						ballLine[i] = Line2D (ball[min_index], obstacle[i].obstacleCoordinate);
						double tempRange = ballLine[i].range(); 
						cout << "ball to obstacle "<<i<<" created with distance "<< tempRange<<"\n";
					}

					//Min Obstacle 
					double minObstacle = ballLine[0].range();
					int minObstacleIndex = 0;
					for(int i=0; i<6; i++){
						if(ballLine[i].range() < minObstacle){
							minObstacle = ballLine[i].range();
							minObstacleIndex = i;
						}
					}
					cout << "Min Obstacle Is " << obstacle[minObstacleIndex].obstacleCoordinate.x <<","<< obstacle[minObstacleIndex].obstacleCoordinate.y <<"\n";

					/*OBSTACLE AVOIDANCE AND CALCULATE WHEEL STEPS***************************************************************************/
					//Obstacle Avoidance Starts
					if(rover1.checkIntersect(obstacle[minObstacleIndex], path[min_index])){
						rover1.headingRange=rover1.headingRange=rover1.calcHeading(ball[min_index],rover1.coordinate,rover1.direction);
						rover1.headingRange=rover1.obstacleAvoidance(obstacle[minObstacleIndex], ball[min_index]);
					}

					else{
						rover1.headingRange=rover1.calcHeading(ball[min_index],rover1.coordinate,rover1.direction);
					}

					double headingInRevolutions  = steps*rover1.headingRange.heading * (1/(10*PI))*((32*PI)/360);
					double rangeInRevolutions = steps*rover1.headingRange.range * 0.508/(10*PI);


					/*MOTOR CONTROL STARTS HERE***************************************************************************/

					// Convert to Left and Right Wheel in Revolutions
					ST400.leftWheel=0;
					ST400.rightWheel=0;

					/*ADDED HERE*/
					if(headingInRevolutions < 0){   // If ClockWise. LEFTWHEEL = MORE.
						ST400.leftWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;
						ST400.rightWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
						motor.Write(&ST400, sizeof(ST400));
						headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

						//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
						motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
						if(status==1)
						{
							printf("Motion completed...\n");
						}
						//motor.Write(&ST400, sizeof(ST400));

					}


					else if(headingInRevolutions > 0){   // If CounterClockWise. RIGHTWHEEL = MORE.
						ST400.leftWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
						ST400.rightWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;
						motor.Write(&ST400, sizeof(ST400));
						headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

						//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
						motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
						if(status==1)
						{
							printf("Motion completed...\n");
						}

					}


					else{  // If Straight.
						ST400.rightWheel = (rangeInRevolutions<5000) ? rangeInRevolutions : 5000;
						ST400.leftWheel = (rangeInRevolutions<5000) ? rangeInRevolutions : 5000;

						if(motor.TestForData() == 0 && ST400.rightWheel!=0){
							motor.Write(&ST400,sizeof(ST400)); 
							printf("Going Straight: wrote identical data to both wheels\n");
							//Reset these commands to 0 after sending the instruction
							//so the motors run for the input steps only once
							ST400.leftWheel=0;
							ST400.rightWheel=0;

							//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
							motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
							if(status==1)
							{
								printf("Motion completed...\n");
							}

						}

					}


					if(motor.TestForData()==0)
					{
						printf("Pipe empty\n");
					}
				}


			}


			else{  // pursue endLine. 

				rover1.taskStatus=0;   // PURSUING ENDLINE - RESET FETCHBALL STATUS.

				CPipe pMainPipe("MainPipe",1024);

				pMainPipe.Read(&roverData,sizeof(roverData));

				rover1.coordinate.x = roverData.posXbot;
				rover1.coordinate.y = roverData.posYbot;
				rover1.direction = roverData.g_angle;

				double ballCoord[6][2];  
				for(int k=0;k<6;k++){
					for(int m=0;m<2;m++){
						ballCoord[k][m]=roverData.balls[k][m];
					}
				}	

				Point2D ball[6];
				for(int i=0;i<6;i++){
					ball[i]=Point2D(ballCoord[i][0], ballCoord[i][1]);
					cout << "Ball "<<i<<" created with coordinates "<<ball[i].x<< " , "<<ball[i].y<<"\n";
					// RANDY: Display ALL items in ball coordinate array. ie same fashion as couts above.
				}

				double obstacleCoord[6][2];
				for(int k=0;k<6;k++){
					for(int m=0;m<2;m++){
						obstacleCoord[k][m]=roverData.balls[k][m];
					}
				}

				Obstacle obstacle[6];
				for(int i=0;i<6;i++){
					Point2D temp(obstacleCoord[i][0], obstacleCoord[i][1]);
					obstacle[i].obstacleCoordinate=temp;
					obstacle[i].radius = 5; // radius of bottle. Supply me with this also
					cout << "Obstacle "<<i<<" created with coordinates "<<obstacle[i].obstacleCoordinate.x<< " , "<<obstacle[i].obstacleCoordinate.y<<"\n";
				}

				Point2D endlineCoordinate(240, 720);
				Line2D endLinePath = Line2D(rover1.coordinate, endlineCoordinate);
				double headingInRevolutions,rangeInRevolutions;
				/*OBSTACLE AVOIDANCE AND CALCULATE WHEEL STEPS***************************************************************************/
				//Obstacle Avoidance Starts
				for(int i = 0; i <5; i++){
					if(rover1.checkIntersect(obstacle[i], endLinePath)){
						rover1.headingRange=rover1.headingRange=rover1.calcHeading(endlineCoordinate,rover1.coordinate,rover1.direction);
						rover1.headingRange=rover1.obstacleAvoidance(obstacle[i], endlineCoordinate);

					}

					else{
						rover1.headingRange=rover1.calcHeading(endlineCoordinate,rover1.coordinate,rover1.direction);
					}

					headingInRevolutions  = steps*rover1.headingRange.heading * (1/(10*PI))*((32*PI)/360);
					rangeInRevolutions = steps*rover1.headingRange.range * 0.508/(10*PI);
				}

				/*MOTOR CONTROL STARTS HERE***************************************************************************/

				// Convert to Left and Right Wheel in Revolutions
				ST400.leftWheel=0;
				ST400.rightWheel=0;


				if(headingInRevolutions < 0){   // If CounterClockWise. LEFTWHEEL = MORE.
					ST400.leftWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;
					ST400.rightWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
					motor.Write(&ST400, sizeof(ST400));
					headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

					//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
					motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
					if(status==1)
					{
						printf("Motion completed...\n");
					}
					//motor.Write(&ST400, sizeof(ST400));

				}


				else if(headingInRevolutions > 0){   // If ClockWise. RIGHTWHEEL = MORE.
					ST400.leftWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;
					ST400.rightWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
					motor.Write(&ST400, sizeof(ST400));
					headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

					//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
					motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
					if(status==1)
					{
						printf("Motion completed...\n");
					}

				}


				else{  // If Straight.
					ST400.rightWheel = (rangeInRevolutions<5000) ? rangeInRevolutions : 5000;
					ST400.leftWheel = (rangeInRevolutions<5000) ? rangeInRevolutions : 5000;

					if(motor.TestForData() == 0 && ST400.rightWheel!=0){
						motor.Write(&ST400,sizeof(ST400)); 
						printf("Going Straight: wrote identical data to both wheels\n");
						//Reset these commands to 0 after sending the instruction
						//so the motors run for the input steps only once
						ST400.leftWheel=0;
						ST400.rightWheel=0;

						//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
						motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
						if(status==1)
						{
							printf("Motion completed...\n");
						}

					}

				}


				if(motor.TestForData()==0)
				{
					printf("Pipe empty\n");
				}


				if(rover1.coordinate.y>670){
					// open gate


					if (serial.Open(4, 9600))   // port number (1~4) and baud rate   //Open  Serial Port:
					{
						const char* szMessage = "2";    // open gate!
						int nBytesSent = serial.SendData(szMessage, strlen(szMessage));
					}

				}

				char cc = cvWaitKey(1);
				if(cc == 27) break;

			}
		}	//End of debug mode 3 while loop
	}

	//else { //debugging mode 4
	//	cout << "Entered Debugging Mode 4: Track 1 ball with one obstacle\n" ;
	//	CPipe pMainPipe("MainPipe",1024);   // Image Processing Pipe
	//	CPipe motor("MotorData",1024);		// WRITE-ONLY pipeline to ST400Control
	//	CPipe motorDone("Stopped", 1024);	// READ-ONLY pipeline from ST400Control
	//	ST400.leftWheel=0;
	//	ST400.rightWheel=0;
	//	ST400.stop = 0;

	//	if(pMainPipe.TestForData() > 0)
	//		pMainPipe.Write(&roverData,sizeof(roverData));

	//	if(roverData.balls[0][0] > roverData.posXbot){ //ball on right side of field
	//		rover1.headingRange.heading;
	//	}

	//	rover1.coordinate.x = roverData.posXbot;    // RANDY: Here are Rover X coordinates to be displayed
	//	rover1.coordinate.y = roverData.posYbot;	// RANDY: Here are Rover Y coordinates to be displayed
	//	rover1.direction = roverData.g_angle;		// RANDY: Here is the rover's angle to be displayed

	//	if(true) {
	//		double ballx,bally;
	//		if(roverData.balls[0][0] > roverData.posXbot){ //ball on right side of field
	//			ballx = roverData.obstacles[0][0]+100;		// RANDY: x and y coordinate of ball is stored in these variables
	//			bally = roverData.obstacles[0][1];
	//		} else{
	//			ballx = roverData.obstacles[0][0]-100;		// RANDY: x and y coordinate of ball is stored in these variables
	//			bally = roverData.balls[0][1];
	//		}

	//		Point2D ball(ballx, bally);

	//		// Calculate Heading.
	//		rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
	//		double headingInRevolutions  = rover1.headingRange.heading*980/360; // RANDY: Display the variable headingInRevolutions (rename to turning Steps in GUI)

	//		// Calculate Range.
	//		double rangeInRevolutions = steps*rover1.headingRange.range * 0.508/(10*PI); // RANDY: Display the rangeInRevolutions and rename to range steps in GUI.

	//		/* Motor control starts here ************************************************************************/

	//		if(headingInRevolutions < -50){   // If ClockWise. LEFTWHEEL = MORE.
	//			//				double turnRate;
	//			ST400.leftWheel =  headingInRevolutions;
	//			ST400.rightWheel = -headingInRevolutions;


	//			//ST400.leftWheel = ST400.leftWheel/2.0;
	//			//ST400.rightWheel = -ST400.leftWheel;

	//			motor.Write(&ST400, sizeof(ST400));
	//			headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

	//			//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE.	
	//			motorDone.Read(&status,sizeof(status));		//Empty pipe to prevent infinite loop

	//			while(motorDone.TestForData() != 0){  // Wait until done. - Added.
	//			}
	//		}


	//		else if(headingInRevolutions > 50){   // If CounterClockWise. RIGHTWHEEL = MORE.
	//			//ST400.leftWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
	//			//ST400.rightWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;

	//			ST400.leftWheel =  -headingInRevolutions;
	//			ST400.rightWheel =  headingInRevolutions;

	//			motor.Write(&ST400, sizeof(ST400));
	//			headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.
	//			//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
	//			motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop

	//			while(motorDone.TestForData() != 0){  // Wait until done. - Added.
	//			}
	//		}

	//		//if(headingInRevolutions==0){  // If Straight.
	//		ST400.leftWheel =  rangeInRevolutions;
	//		ST400.rightWheel =  rangeInRevolutions;


	//		if(motor.TestForData() == 0 && ST400.rightWheel!=0){
	//			motor.Write(&ST400,sizeof(ST400)); 
	//			printf("Going Straight: wrote identical data to both wheels\n");
	//			//Reset these commands to 0 after sending the instruction
	//			//so the motors run for the input steps only once
	//			ST400.leftWheel=0;
	//			ST400.rightWheel=0;

	//			//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
	//			motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
	//			while(motorDone.TestForData() == 0){  // Wait until done. - Added.
	//			}

	//		}




	//		if(motor.TestForData()==0)
	//		{
	//			printf("Pipe empty\n");
	//		}

	//		char cc = cvWaitKey(1);
	//		if(cc == 27) break;
	//	}
	//	//
	//	while(true){

	//		char c = cvWaitKey(1);
	//		if(c == 27)break;

	//		CPipe pMainPipe("MainPipe",1024);

	//		pMainPipe.Read(&roverData,sizeof(roverData));

	//		rover1.coordinate.x = roverData.posXbot;    // RANDY: Here are Rover X coordinates to be displayed
	//		rover1.coordinate.y = roverData.posYbot;	// RANDY: Here are Rover Y coordinates to be displayed
	//		rover1.direction = roverData.g_angle;		// RANDY: Here is the rover's angle to be displayed

	//		if(roverData.balls.empty()){
	//			cout<<"Cant find anymore balls"<<endl;
	//			break;
	//		} else {

	//			double ballx = roverData.balls[0][1];		// RANDY: x and y coordinate of ball is stored in these variables
	//			double bally = roverData.balls[0][2];

	//			Point2D ball(ballx, bally);


	//			//double obstacleCoord[1][1];
	//			//obstacleCoord[0][0]=roverData.obstacles[0][0];
	//			//Obstacle obstacle[1][1];
	//			//Point2D temp(obstacleCoord[0][0], obstacleCoord[0][1]);
	//			//obstacle[0].obstacleCoordinate=temp;
	//			//obstacle[0].radius = 10; 
	//			//
	//			//Line2D destinationPath = Line2D(rover1.coordinate, ball);

	//			//if(rover1.checkIntersect(obstacle[minObstacleIndex], path[min_index])){
	//			//	rover1.headingRange=rover1.headingRange=rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
	//			//	rover1.headingRange=rover1.obstacleAvoidance(obstacle[0], ball);
	//			//}

	//			//else{
	//			//	rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
	//			//}

	//			// Calculate Heading.
	//			rover1.headingRange = rover1.calcHeading(ball,rover1.coordinate,rover1.direction);
	//			double headingInRevolutions  = rover1.headingRange.heading*980/360; // RANDY: Display the variable headingInRevolutions (rename to turning Steps in GUI)

	//			// Calculate Range.
	//			double rangeInRevolutions = steps*rover1.headingRange.range * 0.508/(10*PI); // RANDY: Display the rangeInRevolutions and rename to range steps in GUI.

	//			/* Motor control starts here ************************************************************************/

	//			if(headingInRevolutions < -50){   // If ClockWise. LEFTWHEEL = MORE.
	//				//				double turnRate;
	//				ST400.leftWheel =  headingInRevolutions;
	//				ST400.rightWheel = -headingInRevolutions;


	//				//ST400.leftWheel = ST400.leftWheel/2.0;
	//				//ST400.rightWheel = -ST400.leftWheel;

	//				motor.Write(&ST400, sizeof(ST400));
	//				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.

	//				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE.	
	//				motorDone.Read(&status,sizeof(status));		//Empty pipe to prevent infinite loop

	//				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
	//				}
	//			}


	//			else if(headingInRevolutions > 50){   // If CounterClockWise. RIGHTWHEEL = MORE.
	//				//ST400.leftWheel = (headingInRevolutions<5000) ? -headingInRevolutions/2 : 5000;
	//				//ST400.rightWheel = (headingInRevolutions<5000) ? headingInRevolutions/2 : 5000;

	//				ST400.leftWheel =  -headingInRevolutions;
	//				ST400.rightWheel =  headingInRevolutions;

	//				motor.Write(&ST400, sizeof(ST400));
	//				headingInRevolutions = 0; // Reset Turning to 0 so that rover will now go straight.
	//				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
	//				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop

	//				while(motorDone.TestForData() != 0){  // Wait until done. - Added.
	//				}
	//			}

	//			//if(headingInRevolutions==0){  // If Straight.
	//			ST400.leftWheel =  rangeInRevolutions;
	//			ST400.rightWheel =  rangeInRevolutions;


	//			if(motor.TestForData() == 0 && ST400.rightWheel!=0){
	//				motor.Write(&ST400,sizeof(ST400)); 
	//				printf("Going Straight: wrote identical data to both wheels\n");
	//				//Reset these commands to 0 after sending the instruction
	//				//so the motors run for the input steps only once
	//				ST400.leftWheel=0;
	//				ST400.rightWheel=0;

	//				//sensorFeedback(motorDone, motor, ST400, serial);// THE WHILE LOOP REPLACED HERE. 
	//				motorDone.Read(&status,sizeof(status));			//Empty pipe to prevent infinite loop
	//				while(motorDone.TestForData() == 0){  // Wait until done. - Added.
	//				}

	//			}




	//			if(motor.TestForData()==0)
	//			{
	//				printf("Pipe empty\n");
	//			}

	//			char cc = cvWaitKey(1);
	//			if(cc == 27) break;
	//		}

	//	} // End of While Loop.



	//}
	//	serial.Close();

}

