#ifndef MYMATH_H
#define MYMATH_H

#include <math.h>


//�������������� ���������� �������
class mathVector2d{
public:
	double x, y;
	mathVector2d(double a = 0, double b = 0) : x(a), y(b) {}
	void setMagnitude(double mag);
	void limitMagnitude(double maxMag);
	void setZero();
	void reverse();
	mathVector2d & operator= (mathVector2d const &a){
		this->x = a.x;
		this->y = a.y;
		return *this;
	}
	mathVector2d & operator+= (mathVector2d const &a){
		this->x = this->x + a.x;
		this->y = this->y + a.y;
		return *this;
	}
	mathVector2d & operator-= (mathVector2d const &a){
		this->x = this->x - a.x;
		this->y = this->y - a.y;
		return *this;
	}
};
mathVector2d operator+(mathVector2d &a, mathVector2d &b);
mathVector2d operator-(mathVector2d &a, mathVector2d &b);

#endif
