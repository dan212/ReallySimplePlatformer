#include "stdafx.h"
#define _USE_MATH_DEFINES
#include "MyMath.h"



void mathVector2d::setMagnitude(double mag){
	double angle;
	if ( !(x == 0 && y == 0) ){
		if (x == 0) {
			if (y > 0) angle = M_PI_2;
			else angle = -1 * M_PI_2;
		}
		else {
			if (x > 0) angle = atan(y / x);
			else angle = atan(y / x) + M_PI;
		}
		x = mag * cos(angle);
		y = mag * sin(angle);
	}
}
void mathVector2d::limitMagnitude(double maxMag){
	double mag = sqrt(pow(x, 2) + pow(y, 2));
	if (mag > maxMag){
		this->setMagnitude(maxMag);
	}
}
/*void mathVector2d::limitCoords(double maxMag){
	if (abs(x) > maxMag) {
		if (x > 0) x = maxMag;
		else x = -1 * maxMag;
	}
	if (abs(y) > maxMag) {
		if (y > 0) y = maxMag;
		else y = -1 * maxMag;
	}
}*/
void mathVector2d::setZero(){
	x = 0;
	y = 0;
}
void mathVector2d::reverse(){
	x *= -1;
	y *= -1;
}

mathVector2d operator+(mathVector2d &a, mathVector2d &b){
	return mathVector2d(a.x + b.x, a.y + b.y);
}
mathVector2d operator-(mathVector2d &a, mathVector2d &b){
	return mathVector2d(a.x - b.x, a.y - b.y);
}