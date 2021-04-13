#include "stdafx.h"
#include "Game.h"

//GameObject
GameObject::GameObject(double x, double y, double w, double h){
	pos = mathVector2d(x, y);
	width = w;
	height = h;
}
GameObject::~GameObject(){}
double GameObject::getWidth(){
	return width;
}
double GameObject::getHeight(){
	return height;
}
mathVector2d GameObject::getPos(){
	return pos;
}
void GameObject::setPos(mathVector2d newPos){
	pos = newPos;
}
void GameObject::setPosX(double x){
	pos.x = x;
}
void GameObject::setPosY(double y){
	pos.y = y;
}

std::vector<float> GameObject::draw(int SWidth, int SHight)
{
	std::vector<float> temp_buff;
	temp_buff.push_back(pos.x / SWidth); //ËÂ
	temp_buff.push_back(pos.y / SHight);
	temp_buff.push_back(0.0f);
	temp_buff.push_back((pos.x / SWidth) + (getWidth() / SWidth)); // ÏÂ
	temp_buff.push_back(pos.y / SHight);
	temp_buff.push_back(0.0f);
	temp_buff.push_back((pos.x / SWidth) + (getWidth() / SWidth)); //ÏÍ
	temp_buff.push_back((pos.y / SHight) + (getHeight() / SHight));
	temp_buff.push_back(0.0f);
	temp_buff.push_back(pos.x / SWidth); //ËÂ
	temp_buff.push_back(pos.y / SHight);
	temp_buff.push_back(0.0f);
	temp_buff.push_back((pos.x / SWidth) + (getWidth() / SWidth)); // ÏÍ
	temp_buff.push_back((pos.y / SHight) + (getHeight() / SHight));
	temp_buff.push_back(0.0f);
	temp_buff.push_back(pos.x / SWidth); //ËÍ
	temp_buff.push_back((pos.y / SHight) + (getHeight() / SHight));
	temp_buff.push_back(0.0f);
	return temp_buff;
}

//MovingObject
MovingObject::MovingObject(double x, double y, double w, double h, double maxV) : GameObject(x, y, w, h){
	maxVel = maxV;
}
mathVector2d MovingObject::getVelocity(){
	return vel;
}
void MovingObject::setVelocity(mathVector2d v){
	vel = v;
}
void MovingObject::setVelocityX(double x){
	vel.x = x;
}
void MovingObject::setVelocityY(double y){
	vel.y = y;
}
void MovingObject::applyForce(mathVector2d force){
	acc += force;
}

//Player
Player::Player(double x, double y, double w, double h, double vlim) : GameObject(x, y, w, h), MovingObject(x, y, w, h, vlim){ 
	spawnPoint.x = x;
	spawnPoint.y = y;
}
void Player::respawn(){
	pos = spawnPoint;
	vel.setZero();
	acc.setZero();
}
void Player::update(){
	vel += acc;
	vel.limitMagnitude(maxVel); // limitCoords - ???
	pos += vel;
	acc.setZero();
}
void Player::applyFriction(double k){
	if (!inAir && vel.x != 0){
		if (abs(vel.x) <= 0.04){
			vel.x = 0;
		}
		else {
			double new_vel_X = k * vel.x;
			mathVector2d new_Vel(new_vel_X, 0);
			vel += new_Vel;
		}
	}
}

std::vector<float> Player::drawColor()
{
	std::vector<float> temp_buff;
	for (int j = 0; j < 6; j++) {
		temp_buff.push_back(0.6f); //ëâ 
		temp_buff.push_back(0.4f);
		temp_buff.push_back(0.6f);
	}
	return temp_buff;
}

void Player::setAirFlag(bool f){
	inAir = f;
}
bool Player::getAirFlag(){
	return inAir;
}
void Player::setSpawnPoint(mathVector2d newSpawn){
	spawnPoint = newSpawn;
}

//TechBlock
TechBlock::TechBlock(double x, double y, double w, double h, int type) : GameObject(x, y, w, h){
	blockType = type;
}
int TechBlock::getType(){
	return blockType;
}
std::vector<float> TechBlock::drawColor()
{
	std::vector<float> temp_buff;
	if (getType() == TechTypes::SPIKE) {
		for (int j = 0; j < 6; j++) {
			temp_buff.push_back(1.0f); //ëâ 
			temp_buff.push_back(0.0f);
			temp_buff.push_back(0.0f);
		}
	}
	else { //Ôèíèø
		for (int j = 0; j < 6; j++) {
			temp_buff.push_back(0.1f); //ëâ 
			temp_buff.push_back(0.9f);
			temp_buff.push_back(0.4f);
		}
	}
	return temp_buff;
}


//SolidBlock
SolidBlock::SolidBlock(double x, double y, double w, double h, double frK) : GameObject(x, y, w, h){
	frictionK = frK;
}
double SolidBlock::getFrictionK(){
	return frictionK;
}
std::vector<float> SolidBlock::drawColor()
{
	std::vector<float> temp_buff;
	for (int j = 0; j < 6; j++) {
		temp_buff.push_back(0.0f); //ëâ 
		temp_buff.push_back(1.0f);
		temp_buff.push_back(0.0f);
	}
	return temp_buff;
}

//Connector
Connector::Connector(GameObject* main, GameObject* extra){
	offset = extra->getPos() - main->getPos();
	mainPart = main;
	extraPart = extra;
}
void Connector::update(){
	if (extraPart != NULL && mainPart != NULL){
		extraPart->setPos(mainPart->getPos() + offset);
	}
}

//BounceBlock
BounceBlock::BounceBlock(double x, double y, double w, double h, double power) : GameObject(x, y, w, h){
	pushPower = power;
}
void BounceBlock::setPower(double p){
	pushPower = p;
}
void BounceBlock::hit(Player* plr, int side){
	mathVector2d power;
	int k = 1;
	switch (side){
	case BlockSides::TOP:
		k *= -1;
	case BlockSides::BOTTOM:
		power.y = k * pushPower;
		break;
	case BlockSides::LEFT:
		k *= -1;
	case BlockSides::RIGHT:
		power.x = k * pushPower;
		break;
	}
	plr->applyForce(power);
}
std::vector<float> BounceBlock::drawColor()
{
	std::vector<float> temp_buff;
	for (int j = 0; j < 6; j++) {
		temp_buff.push_back(0.6f); //ëâ 
		temp_buff.push_back(0.6f);
		temp_buff.push_back(0.1f);
	}
	return temp_buff;
}

//MovingPlatform
MovingPlatform::MovingPlatform(double x0, double y0, double w, double h, double vlim, double x1, double y1, double x2, double y2) : GameObject(x0,y0,w,h),
																																	MovingObject(x0,y0,w,h,vlim),
																																	SolidBlock(x0,y0,w,h)
{
	lineEnds[0] = mathVector2d(x1, y1);
	lineEnds[1] = mathVector2d(x2, y2);
	directionFlag = true;
}
void MovingPlatform::update(){
	mathVector2d dir(lineEnds[1].x - lineEnds[0].x, lineEnds[1].y - lineEnds[0].y);
	double pathLength = sqrt(pow(dir.x, 2) + pow(dir.y, 2));
	double pos2platDist = sqrt(pow(pos.x - lineEnds[1].x, 2) + pow(pos.y - lineEnds[1].y, 2));
	double pos1platDist = sqrt(pow(pos.x - lineEnds[0].x, 2) + pow(pos.y - lineEnds[0].y, 2));

	if (pos1platDist > pathLength){
		directionFlag = false;
	}
	if (pos2platDist > pathLength){
		directionFlag = true;
	}

	if (directionFlag){
		vel += dir;
	}
	else{
		dir.reverse();
		vel += dir;
	}
	vel.limitMagnitude(maxVel);
	pos += vel;
}

std::vector<float> MovingPlatform::drawColor()
{
	std::vector<float> temp_buff;
	for (int j = 0; j < 6; j++) {
		temp_buff.push_back(0.0f); //ëâ 
		temp_buff.push_back(0.0f);
		temp_buff.push_back(1.0f);
	}
	return temp_buff;
}

std::string Player::getClassName()
{
	std::string temp_string = typeid(this).name();
	temp_string.erase(0, 6);
	temp_string.erase(temp_string.size() - 2, 3);
	return temp_string;
}

std::string TechBlock::getClassName()
{
	std::string temp_string = typeid(this).name();
	temp_string.erase(0, 6);
	temp_string.erase(temp_string.size() - 2, 3);
	return temp_string;
}

std::string SolidBlock::getClassName()
{
	std::string temp_string = typeid(this).name();
	temp_string.erase(0, 6);
	temp_string.erase(temp_string.size() - 2, 3);
	return temp_string;
}
 std::string MovingPlatform::getClassName()
 {
	 std::string temp_string = typeid(this).name();
	 temp_string.erase(0, 6);
	 temp_string.erase(temp_string.size() - 2, 3);
	 return temp_string;
 }

 std::string BounceBlock::getClassName()
 {
	 std::string temp_string = typeid(this).name();
	 temp_string.erase(0, 6);
	 temp_string.erase(temp_string.size() - 2, 3);
	 return temp_string;
 }