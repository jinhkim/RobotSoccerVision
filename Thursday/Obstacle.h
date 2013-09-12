#define PI 3.14159265
#pragma once
#include "Point2D.h"
#include "Line2D.h"
#include <math.h>

class Obstacle
{
public:
	Point2D obstacleCoordinate;
	double radius;

	Obstacle(void);
	Obstacle(Point2D, double);
	~Obstacle(void);
};
