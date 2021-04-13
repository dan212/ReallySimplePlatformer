#ifndef GAME_H
#define GAME_H

#include "MyMath.h"
#include <vector>
#include <string>

namespace TechTypes{
	const int SPIKE = 0;
	const int FINISH = 1;
}
namespace BlockSides{
	const int TOP = 0;
	const int RIGHT = 1;
	const int BOTTOM = 2;
	const int LEFT = 3;
}

class GameObject{
protected:
	mathVector2d pos;
	double width, height;
public:
	GameObject(double x, double y, double w, double h);
	virtual ~GameObject();
	double getWidth();
	double getHeight();
	mathVector2d getPos();
	void setPos(mathVector2d newPos);
	void setPosX(double x);
	void setPosY(double y);
	std::vector<float> draw(int SWidth, int SHight);
	virtual std::vector<float> drawColor() = 0;
	virtual std::string getClassName() = 0;
};

class MovingObject : virtual public GameObject{
protected:
	mathVector2d vel, acc;
	double maxVel;
public:
	MovingObject(double x, double y, double w, double h, double vlim);
	mathVector2d getVelocity(); //����� ��?
	void setVelocity(mathVector2d v);
	void setVelocityX(double x);
	void setVelocityY(double y);
	void applyForce(mathVector2d force);
	virtual void update() = 0;
};

class Player : public MovingObject{
protected:
	mathVector2d spawnPoint;
	bool inAir = true;
public:
	Player(double x, double y, double w, double h, double vlim);
	void applyFriction(double k);
	void respawn();
	void setAirFlag(bool f);
	bool getAirFlag();
	void setSpawnPoint(mathVector2d newSpawn);
	void update();
	std::vector<float> drawColor();
	std::string getClassName();
};

class TechBlock : public GameObject{
protected:
	int blockType;
public:
	TechBlock(double x, double y, double w, double h, int type);
	int getType();
	std::vector<float> drawColor();
	std::string getClassName();
};

class SolidBlock : virtual public GameObject{
protected:
	double frictionK;
public:
	SolidBlock(double x, double y, double w, double h, double frK = -0.06);
	double getFrictionK();
	std::vector<float> drawColor();
	std::string getClassName();
};

class Connector{
private:
	GameObject* mainPart;
	GameObject* extraPart;
	mathVector2d offset; //�������� ���.����� ������������ ��������
public:
	Connector(GameObject* main, GameObject* extra);
	void update();
};

class BounceBlock : public GameObject{
private:
	double pushPower;
public:
	BounceBlock(double x, double y, double w, double h, double power = 5); //default power = ? (not sure)
	void setPower(double p);
	void hit(Player* plr, int side);
	std::vector<float> drawColor();
	std::string getClassName();
};

class MovingPlatform : public MovingObject , public SolidBlock{
protected:
	mathVector2d lineEnds[2];
	bool directionFlag;
public:
	MovingPlatform(double x0, double y0, double w, double h, double vlim, double x1, double y1, double x2, double y2);
	void update();
	std::vector<float> drawColor();
	std::string getClassName();
};

#endif
