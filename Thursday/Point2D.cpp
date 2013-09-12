#define PI 3.14159265
#include "Point2D.h"
#include <iostream>
#include <math.h>


using namespace std;

Point2D::Point2D(void)
{
}

Point2D::Point2D(double a, double b)
{
	this->x=a; 
	this->y=b;
}

double Point2D::distance(Point2D b)
{
	return sqrt(pow((x-b.x),2)+pow((y-b.y),2));
}

Point2D::~Point2D(void)
{
}
