#include "stdafx.h"
#include "stdio.h"
#include "tchar.h"
#include "C:\\March01\\ST400Control\\rt.h"

#define __RMV856_TRIGGER_SEQUENCE_ENABLE
#include "ST400NT.h"

//Struct for MotorData from Thursday
struct MotorData{
	double leftWheel;				//Steps motors must travel
	double rightWheel;
	bool end;						//1 = close port and end program
};

struct MotorData ST400;

CProcess p("C:\\March01\\Debug\\Thursday.exe",
	NORMAL_PRIORITY_CLASS,			// priority
	OWN_WINDOW,						// process has its own window					
	ACTIVE							// process is active immediately
	);


int _tmain(int argc, _TCHAR* argv[])
{

	/*------------------------Constants-----------------------------*/
	CPipe motor("MotorData",1024);
	char* address1 = "2";		//corresponds to motor driver 3 on ST400
	char* address2 = "3";		//motor driver 4
	int slew_rate = 500;		//16 to 8500 steps/sec
	int first_rate = 100;		//16 to 8500 steps/sec
	int accel = 1;				//0 to 255 steps/sec^2
	int direction1 = 0;			//Sets the direction the motors will turn
	int direction2 = 1;			//1=CLKwise, 0=CClkwise
	int current = 750;			//In mAs
	int port = 4;				//3 for serial, 4 for bluetooth
	int RightWheelDone = 0;		//Bit 7 set to 1 when motor movement is completed
	int	LeftWheelDone = 0;


	//Attempt to open the port and print an error message if it fails to open
	int i=PortOpen(port, 9600);
	if(i==1)
	{
		printf("Port opened\n");
	}
	else
	{
		printf("Error: Port not opened. \nExiting Program...\n");
		return 0;
	}


	//Configure the motors to half-step mode
	MotorConfigureMode(address1,1);
	MotorConfigureMode(address2,1);

	//Initialize the slew rate, first rate and acceleration
	MotorSpeed(  address1, slew_rate, first_rate,  accel );
	MotorSpeed(  address2, slew_rate, first_rate,  accel );

	//Set the winding current
	MotorSetCurrent(address1,current);
	MotorSetCurrent(address2,current);

	while(1){

		//Await instructions from Trajectory Planning program (Thursday).
		motor.Read(&ST400,sizeof(ST400));
		cout << "read "<< ST400.rightWheel << "\t"<<ST400.leftWheel << endl;


		//Check if end has been set to 1.  If so, close the port and end the program.
		if(ST400.end==true)
		{
			printf("Closing the port and ending the program...\n");
			PortClose();
			return 0;
		}


		//Set the motor directions (Note: This function is currently not working so we are trying to resolve it).
		/*if(MotorDirection( address1 , 0)!=1)
		{
			printf("direction failed to change\n");
			char* error = ::RMVGetErrorMessage();
			printf("error=%s\n",error);
		}
		MotorDirection( address2 , 1);*/


		//Set the number of steps the motors must travel
		MotorNumberStepRel( address1 , ST400.leftWheel);
		MotorNumberStepRel( address2 , ST400.rightWheel);


		//Put motors in jog mode (testing purposes only)
		/*MotorJog ( address1, 1 );
		MotorJog ( address2, 1 );
		MotorChangeJogSpeed ( address1, 300);
		MotorChangeJogSpeed ( address2, 300);*/

		//Tell the motors to start executing the received command
		MotorTriggerON(address1);
		MotorTriggerON(address2);

		//Loop until current command is completed (bit 7 is set to 1)
		//(RightWheelDone & 0x80) != 128 && (LeftWheelDone & 0x80) != 128
		while(RightWheelDone != 385 && LeftWheelDone != 385){
			MotorGetStatusRegister(address1,&LeftWheelDone);
			MotorGetStatusRegister(address2,&RightWheelDone);
		}

		//Reset done variables
		LeftWheelDone = 0;
		RightWheelDone = 0;


		MotorTriggerOFF(address1);
		MotorTriggerOFF(address2);
		
	}
}

