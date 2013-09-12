// Module:		Interface for the ST400NT dynamic link library
// Filename:	ST400NT.H
//
//-----------------------------------------------------------------------------
//
// Description: Public interface for the ST400NT controller functions (to be
//				included inside all application modules dealing with the
//				controllers).
//
//-----------------------------------------------------------------------------
//
// Debug Infos:	You can debug this DLL if You have an application which calls
//				the DLL functions. Define the EXE-file inside "St400NT Files"
//				and "settings", option "debug". Make sure the EXE-file does
//				not use the wrong "ST400NT.DLL" (can be the case îf there is
//				still one available at the default path).
//
//=============================================================================

#ifndef ST400NT_H_INCLUDED
#define ST400NT_H_INCLUDED

typedef unsigned char byte;

//=============================================================================
// Section:		DEFINITIONS
// Description: Definition of constants.
//=============================================================================

#define SET_POSITION	    		        'A'
#define IO_READ 				'B'
#define IO_CONFIG				'C'
#define WAIT_PROG				'D'
#define ENTER_PROG				'E'
#define FIRST_RATE				'F'
#define REPEAT_MOVE				'G'
#define HALT_MOTOR				'H'
#define SOFT_RESET				'I'
#define JOG_MOTOR				'J'
#define LOOP_PROG				'L'
#define KILL_FIFO				'K'
#define STEP_MODE				'M'
#define GO_STEPS_REL   			        'N'
#define POSITION_REQ			        'P'
#define QUIT_PROG				'Q'
#define SPI_CONFIG 				'O'
#define SLEW_RATE				'R'
#define ACCEL_RATE    			'S'
#define GO_ABS_POS   			'T'
#define SPI_WRITE      			'U'
#define SPI_READ 				'V'
#define IO_WRITE   				'W'
#define EXEC_PROG				'X'
#define CHNGE_JOG_SPD			'Y'
#define GET_ADC					'Z'
#define DIR_CW					'+'
#define DIR_CCW					'-'
#define BAUD_RATE				'^'
#define SINGLE_ADR_CHR			'_'
#define COND_IO_WRITE			':'
#define MOTOR_STATUS			'='
#define FIFO_INDEX_CTR			'['
#define FIFO_BYTE_FREE			']'
#define MOTOR_POWER				'*'
#define READ_ENCODER			'{'
#define SET_ENCODER				'}'
#define SET_STEPSFAILS          '#'
#define SAVE_ARRAYTOFIFO          '<'
#define MoveBackSWLimit         '~'
#define Digital8BitsFunction          'o'
#define SequentialMotion              's'

//----- Commnad only for the SPORT232STI
#define Buffer_EEPROM              'w'


//-------------END DEFINES FOR RMV856-ET controller-----------
// Global variables and constants. User Data I/O lines.

#define USR0					1
#define USR1					2
#define USR2					4
#define USR3					8
#define USR4					16
#define USR5					32
#define USR6					64
#define USR7					128
#define USR8					256
#define USR9					512
#define USR10					1024
#define USR11					2048
#define USR12					4096

// AD converter resolution.

#define ADC_12_BIT				0x40
#define ADC_10_BIT				0x20
#define ADC_8_BIT				0x10

// SPI constants.

#define SPI_ENB				      128
#define SPI_16BIT    			       64
#define SPI_MASTER                             32
#define SPI_POLARITY  			        8
#define SPI_PHASE     			        4
#define SPI_SPEED4 				3
#define SPI_SPEED3 				2
#define SPI_SPEED2 				1
#define SPI_SPEED1 				0

//------ Different Stepping Modes Flags------------------

#define HALF_STEP  				1
#define BIPHASIC  				2
#define MONOPHASIC 				4
#define EXT_DRIVER 				8
#define	ENCODER_FDBK			        16
#define	POWER_SAVE				32
#define VEL_PROFILE				64
#define SEEK_HOME				128

//-------- Flag for Set Motor Status Register for RMV856-ET controller---------
#define TRIGGER_EXTRN           256  // Set Enable the Hardware Trigger

// #define EVENT_EXTRN             512  // Set Enable the an Extern Event Capability


#define SEQUENTIAL_MOVE        1024
#define SOFTWARE_TRIGGER       2048

//---------------------END FLAGS MOTOR MODE--------------------
// Motor direction.

#define CKWISE  				1
#define CCKWISE 				0

// RMV856 Status flags.

#define TIGGER_ON	  1   // Set when motor has been triggered
#define PAUSED		  2   // Set when motor is being hold (paused)
#define FULL_STOP	  4   // Set when motor was stopped/abortted											// by hardware or software
#define RUNNING		  8   // set when motor is moving
#define SWITCH_CCW	  16  // Set when CCW limit has been reached
#define SWITCH_CW	  32  // Set when CW limit has been reached
#define AT_HOME		  64  // Set when home switch is on
#define MOTION_CMPLT	  128 // Set when motion has been completed
#define CURRENT_DIR	  256 // "1" set for direction for CW,											// "0" set for direction for CCW
#define MOTOR_STALLED	  512 // "1" set when position error is




/////////////////////////////////////////////////////////////
// This Flags are only for the RMV856-ET controller
/////////////////////////////////////////////////////////////

#ifdef  __RMV856_ET

// New Commands for the RMV856-ET Controller
/*-------------------------------------------------------------
The follwing Defines are only for the RMV856-ET only and will work
with the SPORT232-ET, do not use with ST400NT series

---------------------------------------------------------------*/

#define RMV856ET_Init           'i'
#define RMV856ET_SetCurrent     'c'

#define PAUSE_SOFTWARE          'p' // Set Pause via Software
#define TRIGGER_SOFTWARE        't' // Set Trigger via Software




#define GETDATA_EXTRNEVENT      'g' // Get Data from External Events
#define EnableSequentialMotion  's' // Enable Sequential Motion
#define SEQUENTIAL_TRIGGER      'q' // Enable Sequnetial Trigger ON- OFF for
                                    // Master Pivot Controller
// Additional Motor Flags
#define MOTORTRIG_EXTERNAL   2048 // "1" External Trigger has been set  											// bigger than two steps
#define EVENTTRIG_EXTERNAL   4096 // "1" External Event has been set
#define SEQUENCE_COMPLETED   16384 // "1" Motion Sequential Completed

#endif												// bigger than two steps
//***********************************************************************
#define SETUP_EXTRNEVENT        'e' // Set to Step Comnpare Sequence


// Some other definitions.

#define STALL_DETECT			16



//=============================================================================
// Section:		PROTOTYPES
// Description: Definition of prototypes.
//=============================================================================

// Define ITCAPI as DLL export.

#ifdef __BUILDING_THE_DLL
 #define ITCAPI   __declspec( dllexport )
#else
#define ITCAPI __declspec( dllimport )
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Functions for the serial port.

 int  ITCAPI PortOpen( int cPort, int cBaud );
 int  ITCAPI PortClose( void );
 int  ITCAPI PortConfigure( void );
 int  ITCAPI RMVGetPort( void );
 long ITCAPI RMVGetBaudRate( void );
 int  ITCAPI RMVChangeBaudRate( long NewSpeed );
 int  ITCAPI WriteCommandtoRMV856( char* Order );
 long ITCAPI RMVGetValue( void );
 int  ITCAPI GetLastestCommand( char* LastCmd );



 int ITCAPI GetDllVesrsion(char * Version);

 // Error functions.

  ITCAPI char* RMVGetErrorMessage( void );
 int   ITCAPI  RMVGetErrorMessageVb( char* Error );
 int   ITCAPI  RMVGetNumberError( void );

// Addressing functions.

 int  ITCAPI  CheckControllerAddress( char* ICAddress );
 int  ITCAPI  GetConnectedControllers( char* StepperControllers );

// Digital and analog functions.

 int  ITCAPI  ConfigureDigitalUserIO( char* address, int gUSerCnf );
 int  ITCAPI  ConfigureSPI( char* address, int SPIcnf );
 int  ITCAPI ReadDigitalUserIO( char* address, int* value );
 int  ITCAPI ReadSPI( char* address, long* value );
 int  ITCAPI  GetADCchannel( char* address, int resolution, int channel,
							int* adc_value );
 int  ITCAPI  WriteDigitalUserIO( char* address, int gUserData );
 int  ITCAPI WriteSPI( char* address, int gSPIData );

// Winding current functions.

 //int ITCAPI ReadAllChannelsADC( char * address,int resolution, int * values);


 int ITCAPI  MotorSetCurrent( char* address, int current );

// Fixed speed.

 int ITCAPI  GetFixedSpeed( int SpeedWanted );

// Shaft encoder functions.

 int ITCAPI  EncoderSetPosition( char* stnadr, long  NewPosition );
 int ITCAPI  EncoderGetPosition( char* stnadr, long *position );
 int ITCAPI  GetMotorDirection( char* stnadr );
 int ITCAPI  MotorEncoderFailsStep( char* adrstn, int  NumberOfStepsFails );

// Motor functions.

int  ITCAPI   MotorStationsInit( void );

//----------------- Sequential Movement Functions----------------------------
//                Disabled for RMV856-ET Kernel Versions 9.XX
//                                and
//               Versions  10.XX
//---------------------------------------------------------------------------
 int ITCAPI   MotorSequentialMotionEnable( char * AddressPivotController ,byte Multi_Sequence, byte Controllers_Enabled,
                                               byte MotionStack_ADDR01, byte MotionStack_ADDR23);
 int ITCAPI   MotorSequentialMotionNewSequence( char * AddressPivotController ,byte Multi_Sequence,
                                              byte MotionStack_ADDR01, byte MotionStack_ADDR23);
 int ITCAPI   MotorSequentialMotionTriggerON ( char * MasterPivotControllers );
 int ITCAPI   MotorSequentialMotionTriggerOFF ( char * MasterPivotControllers );
 int ITCAPI   MotorSequentialMotionDisable ( char * AddressPivotController, byte ControllersToBeDisabled);
 int ITCAPI   MotorSequentialMotionGetMCmpltedFlag( char * addr_mastercontroller, int * Flag);
 int ITCAPI   MotorSequentialMotionGetSequenceCounter( char * addr_mastercontroller, int * Counter);
  //-------------------------------------END SEQUENTIAL MOVEMENT FUNCTIONS---------
 int  ITCAPI   MotorTriggerON( char* adrlist );
 int  ITCAPI   MotorGetPosition(char* address, long* position );
 int  ITCAPI   MotorTriggerOFF( char* adrlist );
 int  ITCAPI   MotorPauseON( char* adrlist );
 int  ITCAPI   MotorPauseOFF( char* adrlist );
 int  ITCAPI   MotorSetPosition( char* adrlist, long  NewPosition );
 int  ITCAPI   MotorPowerON( char* adrlist, int PowerDownTime );
 int  ITCAPI   MotorPowerOFF( char* adrlist );
 int  ITCAPI   MotorDirection( char* address, int direction );
 int  ITCAPI   MotorNumberStepRel( char* address, long steps_to_go );
 int  ITCAPI   MotorGoAbsPos( char* address, long abs_position );
 int  ITCAPI   MotorJog( char* address, int direction );
 int  ITCAPI   MotorStop( char* address );
 int  ITCAPI   MotorSoftReset( char* address );
 int  ITCAPI   MotorSlewRate( char* address, int slew_rate );
 int  ITCAPI   MotorFirstRate( char* address, int first_rate );
 int  ITCAPI   MotorAccelDecel( char* adrlist, int acceldecel );
 int  ITCAPI   MotorChangeJogSpeed( char* adrlist, int jog_speed );
 int  ITCAPI   MotorRepeat( char* address );
 int  ITCAPI   MotorSpeed( char* address, int slew_rate, int first_rate, int accel );
 int  ITCAPI   MotorWait( char* address, unsigned int delay );
 int  ITCAPI   MotorGetStatusRegister( char* address, int* Status );

// Configuration motion mode.

 int ITCAPI  MotorConfigureMode( char* address, int Mode );


 // Move motor after limit switch has been reach
int ITCAPI    MotorLimitSWMoveOut( char * address, unsigned int Direction, unsigned int NumberOfSteps);

///////////////////////////////////////////////////////////////////
//     These Fuctions can be called only for the  RMV856-ET is
//     controller
//////////////////////////////////////////////////////////////////

#ifdef __RMV856_ET  && __SPORT232ST

//--------- Set Up External Events Procedures---------------


int ITCAPI SetUpExtrnEventProcess( char * stnadr, int WhichProcess, int FirstVariable,
                                   int SecondVariable, int ThirdVariable );
/*-----------------------------------------------------------------------------
 This Function will set up the task for the external Event.
Parameters:
  char * stnadr    :addrees of the RMV856 Controller, No Multi Addrress
                    is allowed
  int WhichProcess : this tell to the RMV856 which functions will execute
                     when the External Event signal is applied
                     1 : Analog to Digital Converter
                     2 : STORE SHAFT ENCODER COUNTER
                     3 : NO IMPLEMENTED IN THIS VERSION
                     4 : NO IMPLEMENTED IN THIS VERSION
  int FisrtVariable :  First parameter for the process selected.
                     I) Process = 1 (ADC) ==> Resolution of the ADC
                        a) FisrtVariable = 10 ; Means 10 bit ADC Resolution;
                        b) FisrtVariable = 12 ; Means 12 bit ADC Resolution;
                     II) Process = 2 Shaft Encoder ==> 0

  int SecondVariable :  Second parameter for the process selected.
                     I) Process = 1 (ADC) ==> How many channels the ADC will be
                                              read, minimum =1; maximum =2;
                     II) Process =2 Shaft Encoder ==> 0
  int ThirdVariable : Third parameter for the process selected.
                      I) Process = 1 (ADC) == 0 ; This variable is not applicable
                                                  for the Process = 1;
                      II) Process = 2 Shaft Encoder ==>0;This variable is not
                                                         applicable for the
                                                         Process = 2;


-----------------------------------------------------------------------------*/
int ITCAPI GetExtrnEventData( char * stnadr,int WhichProcess,int HowManyData, int * Data );
/*--------------------------------------------------------------------------------
This Function will return the data from the Process selected.

 Parameters:
char * stnadr: Address of the RMV856 Controller, No Multi Addrress id not
allowed. When a Process 1 or is selected,  the valid address for the RMV856 controller
must be "0", or "4", or "8", or "12" .:

int HowManyData: How may data need to read. ( In the case of ADC Event
                 this variable means how many channels will be downloaded.
                 In case of Shaft Encoder Event HowManyData needs to be =1;

int * Data : In this pointer to integer  the Data will be stored.

---------------------------------------------------------------------------*/

 int  ITCAPI  SPORT232ST_Init ( void);
 int  ITCAPI  SPORT232ST_SetCurrent(char* address, int current);
 int  ITCAPI  SPORT232SR_TriggerON( char *addrlist);
 int  ITCAPI  SPORT232ST_TriggerOFF (char *addrlist);
 int  ITCAPI  SPORT232ST_PauseON  (char *addlist);
 int  ITCAPI  SPORT232ST_PauseOFF( char* adrdrlist);
 int  ITCAPI  SPORT232ST_ConfigureDigitalUserIO(char* address , int USerCnf );
 int  ITCAPI  SPORT232ST_WriteDigitalUserIO( char* address, int gUserData );
 int  ITCAPI  SPORT232ST_ReadDigitalUserIO( char* address, int* value );
 int  ITCAPI  SPORT232ST_GetADCchannel( char* address, int channel,int* adc_value );

 int  ITCAPI  SPORT232STI_EEPROM_Utility(char * address, int Mode, int Initial_addr,
                                         int End_addr, int * Report_Addr );

 #endif


 /*------------------------------------------------------------------
                        FIFO functions.
 --------------------------------------------------------------------*/
 int  ITCAPI  FIFOMotorSlewRate( char* adrctrl, int slew_rate, int* FifoFull );
 int  ITCAPI  FIFOMotorFirstRate( char* adrctrl, int first_rate, int* FifoFull );
 int  ITCAPI  FIFOMotorSlope( char* adrctrl, int acceldecel, int* FifoFull );
 int  ITCAPI  FIFOMotorRepeat( char* adrctrl, int* FifoFull );
 int  ITCAPI  FIFOMotorWait( char* adrctrl , unsigned int delay, int* FifoFull );
 int  ITCAPI  FIFOWriteUserIO( char* stnadr, int UserIOData, int* FifoFull );
 int  ITCAPI  FIFOWriteSPI( char* stnadr, int SPI_data, int* FifoFull );
 int  ITCAPI  FIFOMotorNumberStepRel( char* stnadr, long steps_to_go, int* FifoFull );
 int  ITCAPI  FIFOMotorGoAbsPos( char* stnadr, long steps_to_go, int* FifoFull );
 int  ITCAPI  FIFOReset( char* stnadr );
 int  ITCAPI  FIFOGetIndexCounter( char* stnadr, int* CommandCounter );
 int  ITCAPI  FIFOGetRemainingBytes( char* stnadr, int* BytesFree );
 int  ITCAPI  ProgramLoop( char* stnadr );
 int  ITCAPI  EnterProgMode( char* adrlist );

 int  ITCAPI  ExecStoredProg( char* adrlist );

//-------------END FIFO FUNCTIONS---------------------------------------------
//----------------------STEP COMPARE SEQUENCE FUNTIONS
#ifdef __RMV856_TRIGGER_SEQUENCE_ENABLE
  #define TRIGGER_SEQUENCE_MAXITEMS    64
//Motor Configure FLAG
#define STEP_COMPARE_TRIGGER        512  // Set Enable Sequence Step Compare

//Motor Status Flags
#define TRIGGER_SEQUENCE_CMPLT        4096

#define TRIGGER_STEPS_SEQUENCE_COMPARE     'r' // Enable Sequential Step Compare
#define GET_PNTR_SCOMAPARED    'g' // Get Data from External Events
#define SET_PULSE_DELAY        'w'

  int  ITCAPI MotorSetTriggerSequence (char* stnadr,  unsigned short * Array_Points, unsigned char NumberOfPoints );
  int  ITCAPI MotorSetTriggerSequenceEnable(char* stnadr,  unsigned char  enable);
  int  ITCAPI MotorDisableTriggerSequence(char* stnadr );

  int  ITCAPI MotorSetTriggerSequencePulseAttribute(char* stnadr, unsigned char Pulse_Output, unsigned char Kte_Delay);
  int  ITCAPI MotorGetFlagTriggerSequenceCompleted(char* stnadr,  unsigned char * Seq_Completed );
#endif

/*----------------------------------------------------------------------------
      SPORT232-STI Self Stand Alone Functions
------------------------------------------------------------------------------*/
#ifdef __VERSION10



#endif
//-------------END SPORT232-STI  FUNCTIONS-------------------------------------



#ifdef __cplusplus
}
#endif

#endif




