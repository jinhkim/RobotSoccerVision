#define PI 3.14159265
#pragma once
#include <math.h>

class Point2D
{
public:
	double x, y;
	Point2D(void);
	Point2D(double, double);
	double distance(Point2D);
	~Point2D(void);
};
