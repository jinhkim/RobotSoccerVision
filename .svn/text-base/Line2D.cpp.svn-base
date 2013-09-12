#define PI 3.14159265
#include "Line2D.h"
#include <iostream>
using namespace std;
#include <math.h>

Line2D::Line2D(void)
{
}

Line2D::Line2D(Point2D a, Point2D b)
{
	this->source = a;
	this->destination = b;
}

double Line2D::range()
{
	return sqrt(pow((this->source.x-this->destination.x),2)+pow((this->source.y-this->destination.y),2));
}

Line2D::~Line2D(void)
{
}

double Line2D::ptLineDist(Point2D point) // returns a double in distance
{
	double slope = (this->destination.y - this->source.y)/(this->destination.x - this->source.x);
	//y = slope*x + intercept
	double yIntercept = this->source.y - this->source.x*slope;
	//cout << "ptLineDist is" << abs(-slope*point.x+point.y-yIntercept) / (sqrt(pow(slope,2)+ 1));
	return abs(-slope*point.x+point.y-yIntercept) / (sqrt(pow(slope,2)+ 1));
}