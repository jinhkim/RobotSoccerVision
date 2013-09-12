// ColourDetection.cpp : This detects the yellow balls, and displays the final result on screen.
//
#define PI 3.1415
#define HSV_RED 0
#define HSV_GREEN 1
#define HSV_YELLOW 2
#define HSV_BLUE 3
#define HSV_RG 4
#define ON 0
#define OFF 1
#include <cv.h>
#include <cxcore.h>
#include <cvaux.h>
#include <highgui.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <windows.h>
#include "rt.h"  

using namespace std;

bool threshWindow=true,HSVTuner=true;

struct botData{
	double posXbot,
		posYbot,
		g_angle,
		posXball,
		posYball;
		vector<vector<double>> balls;
		vector<vector<double>> obstacles;
};

struct colourData{
	vector<vector<double>> redRanges,
						greenRanges,
						yellowRanges,
						blueRanges;
};

struct colourData hsvRanges;

double calcAngle(CvPoint2D32f roverHead, CvPoint2D32f roverTail){
	double roverAngle;
	double opposite;
	double adjacent;
	 
	opposite = roverHead.y - roverTail.y;
	adjacent = roverHead.x - roverTail.x;
	 
	// If the two points are the same point - this is an error failsafe condition only.
	if(opposite == 0 && adjacent ==0){
		roverAngle = 0;
	}
	 
	bool oSign = opposite >= 0; 
	bool aSign = adjacent >= 0;
	double atanValue = 180.0*abs(atan(opposite/adjacent))/PI;

	 
	//determine which quadrant the incident ray lies in
	//quad I
	if (oSign==true && aSign==true ) {
		roverAngle = atanValue;
	//quad II
	} else if (oSign==true && aSign==false) {
		roverAngle = 180.0 - atanValue;
	//quad III
	} else if (oSign==false && aSign==false) {
		roverAngle = 180.0 + atanValue;
	//quad four
	} else { //oSign==false && aSign==true 
		roverAngle = 360.0 - atanValue;
	}
	return (360.0 - roverAngle);
}


int enable = OFF;

int colour_select;

int red_hue_low=161,red_hue_high=179,
	red_sat_low=79,red_sat_high=255,
	red_val_low=100,red_val_high=255,
	green_hue_low=42,green_hue_high=80,
	green_sat_low=29,green_sat_high=92,
	green_val_low=102,green_val_high=255,
	yellow_hue_low=28,yellow_hue_high=41,
	yellow_sat_low=83,yellow_sat_high=166,
	yellow_val_low=140,yellow_val_high=201,
	blue_hue_low=100,blue_hue_high=122,
	blue_sat_low=137,blue_sat_high=255,
	blue_val_low=109,blue_val_high=255;

int pos_hue_low = red_hue_low,
	pos_hue_high = red_hue_high,
	pos_sat_low = red_sat_low,
	pos_sat_high = red_sat_high,
	pos_val_low = red_val_low,
	pos_val_high = red_val_high;

//-----------------------------------------------------------------------
//red HSV callback functions
void RedLowerHUE(int position){hsvRanges.redRanges[0][0] = position;if(enable==ON)red_hue_low=position;}
void RedUpperHUE(int position){hsvRanges.redRanges[1][0] = position;if(enable==ON)red_hue_high=position;}
void RedLowerSAT(int position){hsvRanges.redRanges[0][1] = position;if(enable==ON)red_sat_low=position;}
void RedUpperSAT(int position){hsvRanges.redRanges[1][1] = position;if(enable==ON)red_sat_high=position;}
void RedLowerVAL(int position){hsvRanges.redRanges[0][2] = position;if(enable==ON)red_val_low=position;}
void RedUpperVAL(int position){hsvRanges.redRanges[1][2] = position;if(enable==ON)red_val_high=position;}
//-----------------------------------------------------------------------
//green HSV callback functions
void GreenLowerHUE(int position){hsvRanges.greenRanges[0][0] = position;if(enable==ON)green_hue_low=position;}
void GreenUpperHUE(int position){hsvRanges.greenRanges[1][0] = position;if(enable==ON)green_hue_high=position;}
void GreenLowerSAT(int position){hsvRanges.greenRanges[0][1] = position;if(enable==ON)green_sat_low=position;}
void GreenUpperSAT(int position){hsvRanges.greenRanges[1][1] = position;if(enable==ON)green_sat_high=position;}
void GreenLowerVAL(int position){hsvRanges.greenRanges[0][2] = position;if(enable==ON)green_val_low=position;}
void GreenUpperVAL(int position){hsvRanges.greenRanges[1][2] = position;if(enable==ON)green_val_high=position;}
//-----------------------------------------------------------------------
//yellow HSV callback functions
void YellowLowerHUE(int position){hsvRanges.yellowRanges[0][0] = position;if(enable==ON)yellow_hue_low=position;}
void YellowUpperHUE(int position){hsvRanges.yellowRanges[1][0] = position;if(enable==ON)yellow_hue_high=position;}
void YellowLowerSAT(int position){hsvRanges.yellowRanges[0][1] = position;if(enable==ON)yellow_sat_low=position;}
void YellowUpperSAT(int position){hsvRanges.yellowRanges[1][1] = position;if(enable==ON)yellow_sat_high=position;}
void YellowLowerVAL(int position){hsvRanges.yellowRanges[0][2] = position;if(enable==ON)yellow_val_low=position;}
void YellowUpperVAL(int position){hsvRanges.yellowRanges[1][2] = position;if(enable==ON)yellow_val_high=position;}
//-----------------------------------------------------------------------
//blue HSV callback functions
void BlueLowerHUE(int position){hsvRanges.blueRanges[0][0] = position;if(enable==ON)blue_hue_low=position;}
void BlueUpperHUE(int position){hsvRanges.blueRanges[1][0] = position;if(enable==ON)blue_hue_high=position;}
void BlueLowerSAT(int position){hsvRanges.blueRanges[0][1] = position;if(enable==ON)blue_sat_low=position;}
void BlueUpperSAT(int position){hsvRanges.blueRanges[1][1] = position;if(enable==ON)blue_sat_high=position;}
void BlueLowerVAL(int position){hsvRanges.blueRanges[0][2] = position;if(enable==ON)blue_val_low=position;}
void BlueUpperVAL(int position){hsvRanges.blueRanges[1][2] = position;if(enable==ON)blue_val_high=position;}



int main(int argc, char* argv[])
{	
	
	vector<double> temp;
	for(int i = 0; i < 3; i++){
		temp.push_back(0);
	}
	hsvRanges.redRanges.push_back(temp);
	hsvRanges.redRanges.push_back(temp);
	hsvRanges.greenRanges.push_back(temp);
	hsvRanges.greenRanges.push_back(temp);
	hsvRanges.yellowRanges.push_back(temp);
	hsvRanges.yellowRanges.push_back(temp);
	hsvRanges.blueRanges.push_back(temp);
	hsvRanges.blueRanges.push_back(temp);

	//FreeConsole();
	struct botData roverData;
	roverData.posXbot = 200.0;
	roverData.posYbot = 460.0;
	roverData.balls.clear();
	roverData.obstacles.clear();
	roverData.g_angle = 0.0;

	//CPipe pColourPipe("ColourPipe",1024);
	

	//create pipeline
	CPipe pMainPipe("MainPipe",1024);
	
	
	
	CvCapture* capture=cvCaptureFromCAM(0);
	//CvCapture* capture = cvCaptureFromFile("http://137.82.120.10:8008.mjpeg");
	//CvCapture* capture = cvCaptureFromFile("http://192.168.0.196:8008.mjpeg");
	CvSize capture_size = cvGetSize(cvQueryFrame(capture));
	/*cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 320 );
	cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 240 );*/

	//GUI
	/*int trackbalVal=1;
	int maxVal=1;
	int pos = cvGetTrackbarPos("GUI","display_color_view");

	cvCreateTrackbar("GUI","display_color_view",&trackbalVal,maxVal, NULL );
	cvSetTrackbarPos("GUI","display_color_view", 0);*/

	

	cvNamedWindow("display_color_view",1);

	if(threshWindow)cvNamedWindow("ThresholdedImage",1);
	if(HSVTuner)cvNamedWindow("HSV Tuner",1);

	IplImage* playing_field = cvCreateImage(capture_size,8,3);

	IplImage* tunerWindow = cvCreateImage(cvSize(400,300),8,3);

	IplImage* choiceColour = cvCreateImage(capture_size,8,1);

	IplImage* roverCircle = cvCreateImage(capture_size,8,3);

	IplImage* hsv = cvCreateImage(capture_size,8,3);
	IplImage* thresh_red = cvCreateImage(capture_size,8,1);
	IplImage* thresh_green = cvCreateImage(capture_size,8,1);
	IplImage* thresh_yellow = cvCreateImage(capture_size,8,1);
	IplImage* thresh_blue = cvCreateImage(capture_size,8,1);
	IplImage* thresh = cvCreateImage(capture_size,8,1);
	IplImage* thresh_total = cvCreateImage(capture_size,8,1);
	IplImage* lines = cvCreateImage(capture_size,8,3);
	IplImage* boxes = cvCreateImage(capture_size,8,3);
	IplImage* boxes2 = cvCreateImage(capture_size,8,3);
	IplImage* roverBox = cvCreateImage(capture_size,8,3);

	IplImage* balls = cvCreateImage(capture_size,8,3);
	IplImage* obstacles = cvCreateImage(capture_size,8,3);


	IplImage *thresh_contour = cvCreateImage(capture_size,8,1);
	IplImage *thresh_contour2 = cvCreateImage(capture_size,8,1);
	IplImage *thresh_y_contour = cvCreateImage(capture_size,8,1);
	IplImage *thresh_b_contour = cvCreateImage(capture_size,8,1);


	//storage for contours
	CvMemStorage *red_storage = cvCreateMemStorage(0);
	cvClearMemStorage(red_storage);
	CvMemStorage *green_storage = cvCreateMemStorage(0);
	cvClearMemStorage(green_storage);
	CvMemStorage *blue_storage = cvCreateMemStorage(0);
	cvClearMemStorage(blue_storage);
	CvMemStorage *yellow_storage = cvCreateMemStorage(0);
	cvClearMemStorage(yellow_storage);
	CvMemStorage *rg_storage = cvCreateMemStorage(0);
	cvClearMemStorage(rg_storage);


	//storage for CvBox2D objects
	CvMemStorage *min_rect_storage = cvCreateMemStorage(0);
	cvClearMemStorage(min_rect_storage);
	CvMemStorage *min_rect_storage2 = cvCreateMemStorage(0);
	cvClearMemStorage(min_rect_storage2);
	CvMemStorage *min_rect_storage_balls = cvCreateMemStorage(0);
	cvClearMemStorage(min_rect_storage_balls);
	CvMemStorage *min_rect_storage_obs = cvCreateMemStorage(0);
	cvClearMemStorage(min_rect_storage_obs);
	CvMemStorage *min_rect_storage_rg = cvCreateMemStorage(0);
	cvClearMemStorage(min_rect_storage_rg);

	CvSeq *firstContour = NULL, *greenContour = NULL,
		*yellowContour = NULL, *blueContour = NULL;
	CvSeq *redgreenContour = NULL;
	CvBox2D boxbot,boxredbot,boxgreenbot, boxrgbot,//these are the final CvBox2Ds
		rgBox;//temporary CvBox2D for red/green contour
	vector<CvBox2D> boxballs,boxobs;


	IplImage* frame;
	vector<CvRect> bound_boxes;
	int num_lines = 0,num_boxes = 0, num_circles = 0,
		num_balls = 0, num_obs = 0;
	int i=0;
	int j = 0;
	float w2 = 0.0, h2 = 0.0, cx = 0.0, cy = 0.0;
	CvPoint2D32f box_red_pts[4],box_green_pts[4],box_rg_pts[4];

	double roverAngle = 0.0;

	CvPoint pt1 = cvPoint(170,2);
	CvPoint pt2 = cvPoint(570,474);

	CvRect field = cvRect(pt1.x,pt1.y,pt2.x-170,pt2.y);
	//CvRect main_field = cvRect(pt1.x,pt1.y,pt2.x-170,pt2.y);
	
	cvRectangle(playing_field,pt1,pt2,cvScalar(255,0,255),1,8);

	//ofstream output;
	//output.open("output.txt");

	//DEFAULT THRESHOLD VALUES
	CvScalar red_lower = cvScalar(161, 79, 100);
	CvScalar red_upper = cvScalar(180, 255, 255);

	CvScalar green_lower = cvScalar(42, 29, 102);
	CvScalar green_upper = cvScalar(80, 92, 255);

	CvScalar yellow_lower = cvScalar(28, 83, 140);
	CvScalar yellow_upper = cvScalar(41, 166, 201);

	CvScalar blue_lower = cvScalar(100, 137, 109);
	CvScalar blue_upper = cvScalar(122, 255, 255);

	hsvRanges.redRanges[0][0] = 161; hsvRanges.redRanges[0][1] = 79; hsvRanges.redRanges[0][2] = 100;
	hsvRanges.redRanges[1][0] = 180; hsvRanges.redRanges[1][1] = 255; hsvRanges.redRanges[1][2] = 255;

	hsvRanges.greenRanges[0][0] = 42; hsvRanges.greenRanges[0][1] = 29; hsvRanges.greenRanges[0][2] = 102;
	hsvRanges.greenRanges[1][0] = 80; hsvRanges.greenRanges[1][1] = 92; hsvRanges.greenRanges[1][2] = 255;

	hsvRanges.yellowRanges[0][0] = 28; hsvRanges.yellowRanges[0][1] = 83; hsvRanges.yellowRanges[0][2] = 140;
	hsvRanges.yellowRanges[1][0] = 41; hsvRanges.yellowRanges[1][1] = 166; hsvRanges.yellowRanges[1][2] = 201;

	hsvRanges.blueRanges[0][0] = 100; hsvRanges.blueRanges[0][1] = 137; hsvRanges.blueRanges[0][2] = 109;
	hsvRanges.blueRanges[1][0] = 122; hsvRanges.blueRanges[1][1] = 255; hsvRanges.blueRanges[1][2] = 255;

	cvCreateTrackbar("On/Off","HSV Tuner",&enable,1,NULL);

	cvCreateTrackbar("Colour","HSV Tuner",&colour_select,4,NULL);

	cvCreateTrackbar("HUE_low","HSV Tuner",&pos_hue_low,179,NULL);
	cvCreateTrackbar("HUE_high","HSV Tuner",&pos_hue_high,179,NULL);
	cvCreateTrackbar("SAT_low","HSV Tuner",&pos_sat_low,255,NULL);
	cvCreateTrackbar("SAT_high","HSV Tuner",&pos_sat_high,255,NULL);
	cvCreateTrackbar("VAL_low","HSV Tuner",&pos_val_low,255,NULL);
	cvCreateTrackbar("VAL_high","HSV Tuner",&pos_val_high,255,NULL);

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-MAIN LOOP=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	while(true){
	
		if(colour_select == HSV_RED){
			RedLowerHUE(pos_hue_low); RedUpperHUE(pos_hue_high);
			RedLowerSAT(pos_sat_low); RedUpperSAT(pos_sat_high);
			RedLowerVAL(pos_val_low); RedUpperVAL(pos_val_high);
		} 
		else if(colour_select == HSV_GREEN){
			GreenLowerHUE(pos_hue_low); GreenUpperHUE(pos_hue_high);
			GreenLowerSAT(pos_sat_low); GreenUpperSAT(pos_sat_high);
			GreenLowerVAL(pos_val_low); GreenUpperVAL(pos_val_high);
		} 
		else if(colour_select == HSV_YELLOW){
			YellowLowerHUE(pos_hue_low); YellowUpperHUE(pos_hue_high);
			YellowLowerSAT(pos_sat_low); YellowUpperSAT(pos_sat_high);
			YellowLowerVAL(pos_val_low); YellowUpperVAL(pos_val_high);
		}
		else if(colour_select == HSV_BLUE){
			BlueLowerHUE(pos_hue_low); BlueUpperHUE(pos_hue_high);
			BlueLowerSAT(pos_sat_low); BlueUpperSAT(pos_sat_high);
			BlueLowerVAL(pos_val_low); BlueUpperVAL(pos_val_high);
		}
		
		if(enable == ON){
			red_lower = cvScalar(hsvRanges.redRanges[0][0],hsvRanges.redRanges[0][1],hsvRanges.redRanges[0][2]);
			red_upper = cvScalar(hsvRanges.redRanges[1][0],hsvRanges.redRanges[1][1],hsvRanges.redRanges[1][2]);

			green_lower = cvScalar(hsvRanges.greenRanges[0][0],hsvRanges.greenRanges[0][1],hsvRanges.greenRanges[0][2]);
			green_upper = cvScalar(hsvRanges.greenRanges[1][0],hsvRanges.greenRanges[1][1],hsvRanges.greenRanges[1][2]);

			yellow_lower = cvScalar(hsvRanges.yellowRanges[0][0],hsvRanges.yellowRanges[0][1],hsvRanges.yellowRanges[0][2]);
			yellow_upper = cvScalar(hsvRanges.yellowRanges[1][0],hsvRanges.yellowRanges[1][1],hsvRanges.yellowRanges[1][2]);

			blue_lower = cvScalar(hsvRanges.blueRanges[0][0],hsvRanges.blueRanges[0][1],hsvRanges.blueRanges[0][2]);
			blue_upper = cvScalar(hsvRanges.blueRanges[1][0],hsvRanges.blueRanges[1][1],hsvRanges.blueRanges[1][2]);
		}
		
		frame = cvQueryFrame(capture);

		cvCvtColor(frame,hsv,CV_BGR2HSV);

		//get red and green thresholded images
		cvInRangeS(hsv, red_lower, red_upper, thresh_red);
		cvInRangeS(hsv, green_lower, green_upper, thresh_green);
		cvInRangeS(hsv, yellow_lower, yellow_upper, thresh_yellow);
		cvInRangeS(hsv, blue_lower, blue_upper, thresh_blue);


		//cvSetImageROI(thresh_red,field);
		//cvDilate(thresh_red,thresh_red,NULL,1);
		//cvResetImageROI(thresh_red);

		//cvSetImageROI(thresh_green,field);
		//cvSmooth(thresh_green, thresh_green, CV_BLUR, 3, 3);
		//cvResetImageROI(thresh_green);

		cvSetImageROI(thresh_yellow,field);
		//cvErode(thresh_yellow,thresh_yellow,NULL,1);
		cvSmooth(thresh_yellow, thresh_yellow, CV_GAUSSIAN, 3, 3);
		cvDilate(thresh_yellow,thresh_yellow,NULL,2);
		cvResetImageROI(thresh_yellow);
		//cvSetImageROI(thresh_blue,field);
		//cvDilate(thresh_blue,thresh_blue,NULL,1);
		//cvSmooth(thresh_blue,thresh_blue,CV_GAUSSIAN,3,3);
		//cvResetImageROI(thresh_blue);


		//get contoured image of thresh
		cvCopy(thresh_red,thresh_contour,NULL);
		cvSetImageROI(thresh_contour,field);
		cvFindContours(thresh_contour,red_storage,&firstContour,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
		cvResetImageROI(thresh_contour);

		cvCopy(thresh_green,thresh_contour2,NULL);
		cvSetImageROI(thresh_contour2,field);
		cvFindContours(thresh_contour2,green_storage,&greenContour,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
		cvResetImageROI(thresh_contour2);

		cvCopy(thresh_yellow,thresh_y_contour,NULL);
		cvSetImageROI(thresh_y_contour,field);
		cvFindContours(thresh_y_contour,yellow_storage,&yellowContour,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
		cvResetImageROI(thresh_y_contour);

		cvCopy(thresh_blue,thresh_b_contour,NULL);
		cvSetImageROI(thresh_b_contour,field);
		cvFindContours(thresh_b_contour,blue_storage,&blueContour,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
		cvResetImageROI(thresh_b_contour);


		double boxred_area,boxgreen_area,boxrg_area=0;
		CvBox2D boxred,boxgreen;
		boxred.center = cvPoint2D32f(0,0);
		boxgreen.center = cvPoint2D32f(0,0);
		boxred.angle = 0.0;
		boxgreen.angle = 0.0;

		num_boxes = 0;

//---------------------------------------------------------------------------------------
		//draw bounding box for red objects
//---------------------------------------------------------------------------------------
		cvSetImageROI(boxes,field);
		for( ; firstContour != 0; firstContour = firstContour->h_next){

			if(firstContour != NULL){
				boxred = cvMinAreaRect2(firstContour,min_rect_storage);
				boxred_area = (double)(boxred.size.height*boxred.size.width);
				if(boxred_area > 800 && boxred_area < 1400){
					cvBoxPoints(boxred,box_red_pts);
					//cout<<"boxred area: "<<(int)boxred_area<<"\t";
					boxredbot = boxred;
				}
			}

			cvLine(boxes,cvPoint(box_red_pts[0].x,box_red_pts[0].y),
				cvPoint(box_red_pts[1].x,box_red_pts[1].y),cvScalar(100,100,255),1,8,0);
			cvLine(boxes,cvPoint(box_red_pts[1].x,box_red_pts[1].y),
				cvPoint(box_red_pts[2].x,box_red_pts[2].y),cvScalar(100,100,255),1,8,0);
			cvLine(boxes,cvPoint(box_red_pts[2].x,box_red_pts[2].y),
				cvPoint(box_red_pts[3].x,box_red_pts[3].y),cvScalar(100,100,255),1,8,0);
			cvLine(boxes,cvPoint(box_red_pts[3].x,box_red_pts[3].y),
				cvPoint(box_red_pts[0].x,box_red_pts[0].y),cvScalar(100,100,255),1,8,0);
			num_boxes = num_boxes + 4;

			if(num_boxes > 4){
				cvSet(boxes,cvScalar(0,0,0));
				num_boxes = 0;
			}

		}
		cvResetImageROI(boxes);
		cvAdd(boxes,frame,frame);

		num_boxes = 0;

//---------------------------------------------------------------------------------------
		//draw bounding box for green objects
//---------------------------------------------------------------------------------------
		cvSetImageROI(boxes2,field);
		for( ; greenContour != 0; greenContour = greenContour->h_next){

			if(greenContour != NULL){
				boxgreen = cvMinAreaRect2(greenContour,min_rect_storage2);
				boxgreen_area = (double)boxgreen.size.height*boxgreen.size.width;
				
				if(boxgreen_area > 700 && boxgreen_area < 1300){
					cvBoxPoints(boxgreen,box_green_pts);
					//cout<<"boxgreen_area: "<<(int)boxgreen_area<<endl;
					boxgreenbot = boxgreen;
				}
			}else{
				w2 = 0.0;
				h2 = 0.0;
				cx = 0.0;
				cy = 0.0;
			}

			cvLine(boxes2,cvPoint(box_green_pts[0].x,box_green_pts[0].y),
				cvPoint(box_green_pts[1].x,box_green_pts[1].y),cvScalar(100,255,100),1,8,0);
			cvLine(boxes2,cvPoint(box_green_pts[1].x,box_green_pts[1].y),
				cvPoint(box_green_pts[2].x,box_green_pts[2].y),cvScalar(100,255,100),1,8,0);
			cvLine(boxes2,cvPoint(box_green_pts[2].x,box_green_pts[2].y),
				cvPoint(box_green_pts[3].x,box_green_pts[3].y),cvScalar(100,255,100),1,8,0);
			cvLine(boxes2,cvPoint(box_green_pts[3].x,box_green_pts[3].y),
				cvPoint(box_green_pts[0].x,box_green_pts[0].y),cvScalar(100,255,100),1,8,0);

			num_boxes = num_boxes + 4;

			if(num_boxes > 4){
				cvSet(boxes2,cvScalar(0,0,0));
				num_boxes = 0;
			}

		}
		cvResetImageROI(boxes2);
		cvAdd(boxes2,frame,frame);

		num_boxes = 0;
		
		cvSetImageROI(lines,field);
		if(num_lines > 1){
			cvSet(lines,cvScalar(0,0,0));
			num_lines = 0;
		}
		cvLine(lines,cvPoint(boxredbot.center.x,boxredbot.center.y),
			cvPoint(boxgreenbot.center.x,boxgreenbot.center.y),cvScalar(0,255,0),2,8,0);
		num_lines++;
		cvResetImageROI(lines);
		cvAdd(lines,frame,frame);

		num_boxes = 0;
		boxred_area = (double)boxredbot.size.height*boxredbot.size.width;
		boxgreen_area = (double)boxgreenbot.size.height*boxgreenbot.size.width;
		double redx = boxredbot.center.x;
		double redy = boxredbot.center.y;
		double greenx = boxgreenbot.center.x;
		double greeny = boxgreenbot.center.y;
		
/////////////////////////////////////////////////////////////////////////////////////////
//////////combine red and green contours/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
		cvOr(thresh_red,thresh_green,thresh);
		cvSetImageROI(thresh,field);
		cvFindContours(thresh,rg_storage,&redgreenContour,sizeof(CvContour),CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE);
		cvResetImageROI(thresh);
		bool isRover = false;
		//draw bounding box for ROVER
		cvSetImageROI(roverBox,field);
		for(; redgreenContour != 0; redgreenContour = redgreenContour->h_next){
			

			if(redgreenContour != NULL){
				rgBox = cvMinAreaRect2(redgreenContour,min_rect_storage_rg);
				//double roversize = (double)rgBox.size.height*rgBox.size.width;
				double total = boxred_area + boxgreen_area;
				//int diff = abs((int)(boxred_area - boxgreen_area));

				if( total > 1600 && total < 2900 && boxred_area<1400 && boxgreen_area<1300){
					
					
					if(cvPointPolygonTest(redgreenContour,cvPoint2D32f(redx,redy), false)>0
					   && cvPointPolygonTest(redgreenContour,cvPoint2D32f(greenx,greeny), false)>0){
						cvBoxPoints(rgBox,box_rg_pts);
						//cout<<"rgBox area: "<<total<<"\t";
						boxrgbot = rgBox;
						isRover = true;
						
					}
				} else{
					isRover = false;
					continue;
				}
			} else{
				w2 = 0.0;
				h2 = 0.0;
				cx = 0.0;
				cy = 0.0;
			}

			if(num_boxes > 4){
				cvSet(roverBox,cvScalar(0,0,0));
				num_boxes = 0;
			}

			cvLine(roverBox,cvPoint(box_rg_pts[0].x,box_rg_pts[0].y),
				cvPoint(box_rg_pts[1].x,box_rg_pts[1].y),cvScalar(0,100,255),2,8,0);
			cvLine(roverBox,cvPoint(box_rg_pts[1].x,box_rg_pts[1].y),
				cvPoint(box_rg_pts[2].x,box_rg_pts[2].y),cvScalar(0,100,255),2,8,0);
			cvLine(roverBox,cvPoint(box_rg_pts[2].x,box_rg_pts[2].y),
				cvPoint(box_rg_pts[3].x,box_rg_pts[3].y),cvScalar(0,100,255),2,8,0);
			cvLine(roverBox,cvPoint(box_rg_pts[3].x,box_rg_pts[3].y),
				cvPoint(box_rg_pts[0].x,box_rg_pts[0].y),cvScalar(0,100,255),2,8,0);
			num_boxes = num_boxes + 4;

		}
		cvResetImageROI(roverBox);
		cvAdd(roverBox,frame,frame);
		roverAngle = calcAngle(boxredbot.center,boxgreenbot.center);
		if(isRover){
			boxrgbot.center.x += pt1.x;
			boxrgbot.center.y += pt1.y;
			boxrgbot.center.y = 480 - boxrgbot.center.y;
			//cout<<"rover x: "<<(int)boxrgbot.center.x<<"\trover y: "<<(int)boxrgbot.center.y<<endl; 
		}
		
		//cout<<"angle: "<<roverAngle<<endl;
		
		//else lines = NULL;
		//cvRect(posX,posY,100,100);
		//cvAdd(frame,lines,frame);
		
//---------------------------------------------------------------------------------------
		//draw bounding box for yellow balls
//---------------------------------------------------------------------------------------
		i=0;
		cvSetImageROI(balls,field);
		for( ; yellowContour != 0; yellowContour = yellowContour->h_next){
			CvBox2D tempBall;
			vector<double> currentBall;
			if(yellowContour != NULL){
				tempBall = cvMinAreaRect2(yellowContour,min_rect_storage_balls);
				double ballsize = (double)tempBall.size.height*tempBall.size.width;
				if( ballsize > 175 && ballsize < 370){
					boxballs.push_back(tempBall);
					currentBall.push_back(tempBall.center.x);
					currentBall.push_back(tempBall.center.y);
					roverData.balls.push_back(currentBall); //update pipelines
					w2 = boxballs[i].size.width/2;
					h2 = boxballs[i].size.height/2;
					cx = boxballs[i].center.x;
					cy = boxballs[i].center.y;
					roverData.posXball = tempBall.center.x;
					roverData.posYball = tempBall.center.y;
					i++;
					//cout<<"ball"<<i<<" area: "<<(int)ballsize;
				} else{
					w2 = 0.0;
					h2 = 0.0;
					cx = 0.0;
					cy = 0.0;
				}
			} else{
				w2 = 0.0;
				h2 = 0.0;
				cx = 0.0;
				cy = 0.0;
			}

			if(num_balls > 24){
				cvSet(balls,cvScalar(0,0,0));
				num_balls = 0;
			}

			cvLine(balls,cvPoint(cx-w2,cy-h2),cvPoint(cx+w2,cy-h2),cvScalar(0,200,255),2,8,0);
			cvLine(balls,cvPoint(cx+w2,cy-h2),cvPoint(cx+w2,cy+h2),cvScalar(0,200,255),2,8,0);
			cvLine(balls,cvPoint(cx+w2,cy+h2),cvPoint(cx-w2,cy+h2),cvScalar(0,200,255),2,8,0);
			cvLine(balls,cvPoint(cx-w2,cy+h2),cvPoint(cx-w2,cy-h2),cvScalar(0,200,255),2,8,0);
			num_balls = num_balls + 4;

		}
		//cout<<endl;
		cvResetImageROI(balls);
		cvAdd(balls,frame,frame);
		//add offset; offset comes from using ROI
		for(int g=0;g<boxballs.size();g++){
			boxballs.at(g).center.x += pt1.x;
			boxballs.at(g).center.y += pt1.y;
		}
		//cout<<boxballs.size()<<" balls detected";
		
				
		if(boxballs.size() > 0){
			vector<double> temp,areas;
			int ball_count = 0;
			double ballsize;
			//roverData.balls.push_back(temp.push_back((double)boxballs[0].center.x));
			//roverData.balls.push_back(temp.push_back((double)boxballs[0].center.y));
			for(int k = 0;k < boxballs.size();k++){
				ballsize = (double)boxballs[k].size.height*boxballs[k].size.width;
				//if(ballsize > 120 && ballsize < 200){
					areas.push_back(ballsize);
					//cout<<"ball"<<ball_count<<" area: "<<areas[ball_count]<<"\t";
					//output<<"ball"<<ball_count<<" area: "<<areas[ball_count]<<"\t";
					ball_count++;
				//}
				
			}
			//cout<<endl;
		}
		boxballs.clear();

//---------------------------------------------------------------------------------------
		//draw bounding box for blue obstacles
//---------------------------------------------------------------------------------------
		i=0;
		cvSetImageROI(obstacles,field);
		for( ; blueContour != 0; blueContour = blueContour->h_next){
			CvBox2D tempObs;
			vector<double> currentObs;
			if(blueContour != NULL){
				tempObs = cvMinAreaRect2(blueContour,min_rect_storage_obs);
				double obssize = (double)tempObs.size.height*tempObs.size.width;
				
				if(obssize > 270){
					//cout<<"obs"<<i<<": "<<(int)obssize<<" ";
					boxobs.push_back(tempObs);
					currentObs.push_back(tempObs.center.x);
					currentObs.push_back(tempObs.center.y);
					roverData.obstacles.push_back(currentObs);
					w2 = boxobs[i].size.width/2;
					h2 = boxobs[i].size.height/2;
					cx = boxobs[i].center.x;
					cy = boxobs[i].center.y;
					i++;
				}
			} else {
				w2 = 0.0;
				h2 = 0.0;
				cx = 0.0;
				cy = 0.0;
			}

			if(num_obs > 48){
				cvSet(obstacles,cvScalar(0,0,0));
				num_obs = 0;
			}

			cvLine(obstacles,cvPoint(cx-w2,cy-h2),cvPoint(cx+w2,cy-h2),cvScalar(200,200,0),2,8,0);
			cvLine(obstacles,cvPoint(cx+w2,cy-h2),cvPoint(cx+w2,cy+h2),cvScalar(200,200,0),2,8,0);
			cvLine(obstacles,cvPoint(cx+w2,cy+h2),cvPoint(cx-w2,cy+h2),cvScalar(200,200,0),2,8,0);
			cvLine(obstacles,cvPoint(cx-w2,cy+h2),cvPoint(cx-w2,cy-h2),cvScalar(200,200,0),2,8,0);
			num_obs = num_obs + 4;

		}
		cvResetImageROI(obstacles);
		cvAdd(obstacles,frame,frame);
		//cout<<" and "<<boxobs.size()<<" obstacles detected"<<endl;
		//cout<<endl;
		for(int g=0;g<boxobs.size();g++){
			boxobs.at(g).center.x += pt1.x;
			boxobs.at(g).center.y += pt1.y;
		}

		if(boxobs.size() > 0){
			vector<double> temp,areas;
			int obs_count = 0;
			double obssize;

			for(int k = 0;k < boxobs.size();k++){
				obssize = (double)boxobs[k].size.height*boxobs[k].size.width;
				//if(obssize > 390){
					areas.push_back(obssize);
					//cout<<"obs"<<obs_count<<" area: "<<(int)areas[obs_count]<<"\t";
					//output<<"obs"<<obs_count<<" area: "<<(int)areas[obs_count]<<"\t";
					obs_count++;
				//}
				
			}
			//cout<<endl;
			//output<<endl;
		}
		//add offset; offset comes from using ROI
		for(int g=0;g<boxballs.size();g++){
			boxobs.at(g).center.x += pt1.x;
			boxobs.at(g).center.y += pt1.y;
			roverData.obstacles[0][0] += pt1.x;
			roverData.obstacles[0][1] += pt1.y;
		}
		if(!roverData.obstacles.empty()){
			roverData.obstacles[0][0] += pt1.x;
			roverData.obstacles[0][1] += pt1.y;
		}

		//Write coordinate/angle variables to pipeline
		


		boxobs.clear();

		

		//if(num_circles > 1){
		//	cvSet(roverCircle,cvScalar(0,0,0));
		//	num_circles = 0;
		//}
		//cvCircle(roverCircle,cvPoint(boxrgbot.center.x,boxrgbot.center.y),100,cvScalar(0,100,255),2,8);
		//num_circles++;

		//cout<<"rover x: "<<(int)boxrgbot.center.x
		//	<<"\trover y: "<<(int)boxrgbot.center.y
		//	<<"\tangle: "<<(int)roverAngle<<endl;


		//update global variables
		
		roverData.posXbot = (double)boxrgbot.center.x;
		roverData.posYbot = (double)boxrgbot.center.y;
		roverData.g_angle = (double)roverAngle;

		
		if(colour_select == HSV_RED){
			cvSet(choiceColour,cvScalar(0,0,0));
			cvCopy(thresh_red,choiceColour);
			cvSetTrackbarPos("HUE_low","HSV Tuner",red_hue_low);
			cvSetTrackbarPos("HUE_high","HSV Tuner",red_hue_high);
			cvSetTrackbarPos("SAT_low","HSV Tuner",red_sat_low);
			cvSetTrackbarPos("SAT_high","HSV Tuner",red_sat_high);
			cvSetTrackbarPos("VAL_low","HSV Tuner",red_val_low);
			cvSetTrackbarPos("VAL_high","HSV Tuner",red_val_high);
		} 
		else if(colour_select == HSV_GREEN){
			cvSet(choiceColour,cvScalar(0,0,0));
			cvCopy(thresh_green,choiceColour);
			cvSetTrackbarPos("HUE_low","HSV Tuner",green_hue_low);
			cvSetTrackbarPos("HUE_high","HSV Tuner",green_hue_high);
			cvSetTrackbarPos("SAT_low","HSV Tuner",green_sat_low);
			cvSetTrackbarPos("SAT_high","HSV Tuner",green_sat_high);
			cvSetTrackbarPos("VAL_low","HSV Tuner",green_val_low);
			cvSetTrackbarPos("VAL_high","HSV Tuner",green_val_high);
		}
		else if(colour_select == HSV_YELLOW){
			cvSet(choiceColour,cvScalar(0,0,0));
			cvCopy(thresh_yellow,choiceColour);
			cvSetTrackbarPos("HUE_low","HSV Tuner",yellow_hue_low);
			cvSetTrackbarPos("HUE_high","HSV Tuner",yellow_hue_high);
			cvSetTrackbarPos("SAT_low","HSV Tuner",yellow_sat_low);
			cvSetTrackbarPos("SAT_high","HSV Tuner",yellow_sat_high);
			cvSetTrackbarPos("VAL_low","HSV Tuner",yellow_val_low);
			cvSetTrackbarPos("VAL_high","HSV Tuner",yellow_val_high);
		}
		else if(colour_select == HSV_BLUE){
			cvSet(choiceColour,cvScalar(0,0,0));
			cvCopy(thresh_blue,choiceColour);
			cvSetTrackbarPos("HUE_low","HSV Tuner",blue_hue_low);
			cvSetTrackbarPos("HUE_high","HSV Tuner",blue_hue_high);
			cvSetTrackbarPos("SAT_low","HSV Tuner",blue_sat_low);
			cvSetTrackbarPos("SAT_high","HSV Tuner",blue_sat_high);
			cvSetTrackbarPos("VAL_low","HSV Tuner",blue_val_low);
			cvSetTrackbarPos("VAL_high","HSV Tuner",blue_val_high);
		}
		else{
			cvSet(choiceColour,cvScalar(0,0,0));
			cvCopy(thresh,choiceColour);
		}
		

		cvAdd(frame,playing_field,frame);
		cvOr(thresh_green,thresh_red,thresh,NULL);

			
		//cout<<(int)roverData.posXball<<"    "<<(int)roverData.posYball<<endl;
		if(pMainPipe.TestForData() == 0)
			pMainPipe.Write(&roverData,sizeof(roverData));
		roverData.balls.clear();
		roverData.obstacles.clear();

		if(threshWindow)cvShowImage("ThresholdedImage",choiceColour);
		if(HSVTuner)cvShowImage("HSV Tuner",tunerWindow);

		cvShowImage("display_color_view",frame);
		

		char c = cvWaitKey(1);
		if(c == 27) break;//ESC key
		else if(c == 96){ //RESET key
			boxred.center = cvPoint2D32f(0,0);
			boxred.size = cvSize2D32f(0,0);
			boxredbot.center = cvPoint2D32f(0,0);
			boxredbot.size = cvSize2D32f(0,0);
			boxred_area = 0.0;
			boxgreen.center = cvPoint2D32f(0,0);
			boxgreen.size = cvSize2D32f(0,0);
			boxgreenbot.center = cvPoint2D32f(0,0);
			boxgreenbot.size = cvSize2D32f(0,0);
			boxgreen_area = 0.0;
			rgBox.center = cvPoint2D32f(0,0);
			rgBox.size = cvSize2D32f(0,0);
			for(int k=0;k<4;k++)
				box_red_pts[k] = cvPoint2D32f(0,0);
			for(int k=0;k<4;k++)
				box_green_pts[k] = cvPoint2D32f(0,0);
			for(int k=0;k<4;k++)
				box_rg_pts[k] = cvPoint2D32f(0,0);
			cvSet(boxes,cvScalar(0,0,0));
			cvSet(boxes2,cvScalar(0,0,0));
			cvSet(roverBox,cvScalar(0,0,0));
			cvSet(lines,cvScalar(0,0,0));
			cvSet(balls,cvScalar(0,0,0));
			cvSet(obstacles,cvScalar(0,0,0));
			cout<<"\nRESET"<<endl;
		}
	}
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-END OF MAIN LOOP=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	cvReleaseImage(&playing_field);
	cvReleaseImage(&roverCircle);
	//cvReleaseImage(&frame);
	cvReleaseImage(&hsv);
	cvReleaseImage(&thresh_red);
	cvReleaseImage(&thresh_green);
	cvReleaseImage(&thresh_yellow);
	cvReleaseImage(&thresh_blue);
	cvReleaseImage(&thresh);
	cvReleaseImage(&thresh_total);
	cvReleaseImage(&lines);
	cvReleaseImage(&boxes);
	cvReleaseImage(&boxes2);
	cvReleaseImage(&roverBox);
	cvReleaseImage(&balls);
	cvReleaseImage(&obstacles);
	cvReleaseImage(&thresh_contour);
	cvReleaseImage(&thresh_contour2);
	cvReleaseImage(&thresh_y_contour);
	cvReleaseImage(&thresh_b_contour);
	cvReleaseImage(&tunerWindow);
	cvReleaseImage(&choiceColour);
	cvReleaseMemStorage(&red_storage);
	cvReleaseMemStorage(&green_storage);
	cvReleaseMemStorage(&blue_storage);
	cvReleaseMemStorage(&yellow_storage);
	cvReleaseMemStorage(&rg_storage);
	cvReleaseMemStorage(&min_rect_storage);
	cvReleaseMemStorage(&min_rect_storage2);
	cvReleaseMemStorage(&min_rect_storage_balls);
	cvReleaseMemStorage(&min_rect_storage_obs);
	cvReleaseMemStorage(&min_rect_storage_rg);
	cvReleaseCapture(&capture);
	cvDestroyWindow("display_color_view");
	if(HSVTuner)cvDestroyWindow("HSV Tuner");
	if(threshWindow)cvDestroyWindow("ThresholdedImage");
	pMainPipe.~CPipe();
	//pColourPipe.~CPipe();

	return 0;
}

