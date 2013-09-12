#pragma once
#define PI 3.14159265
#include "HeadingRange.h"
#include "Obstacle.h"
#include "Line2D.h"
#include "Point2D.h"
#include <iostream>
using namespace std;
#include <math.h>

class Rover
{
public:
	Point2D coordinate;
	double direction;

	HeadingRange headingRange;

	int taskStatus;
	bool intermediateStatus;
	bool ballStatus;
	bool endLineStatus;

	Rover();
	Rover(Point2D, double); // Takes in Point2D [coordinate] and double [Direction]

	double calcRange(Point2D, Point2D);
	HeadingRange calcHeading(Point2D, Point2D, double);
	bool checkIntersect(Obstacle, Line2D);
	HeadingRange obstacleAvoidance(Obstacle, Point2D);

	~Rover(void);
};
