#ifndef MANAGEMENT_H
#define MANAGEMENT_H

#include "Game.h"
#include <fstream>
#include <Windows.h>
#include <array>
using namespace std;

class GameLevel{
private:
	std::vector<GameObject*> blocks;
	std::vector<Connector*> connectors;
	Player* player;

	std::string levelName;
	mathVector2d gravitation;
	bool finishFlag;
	std::array<double, 4> edgeCoords;

	void checkCollisions();
	void collide(GameObject* block);
public:
	GameLevel();
	~GameLevel();
	int buildLevel(std::string filename);
	bool isFinished();
	mathVector2d getPlayerPos();
	void updateLevelState();
	std::vector<float> drawLevel(int SWidth, int SHight);
	std::vector<float> colourLevel();
	void applyInputs();
	std::string getName();
	std::array<double, 4> getEdgeCoords();

	std::vector<GameObject*> getBlocks();
	std::vector<Connector*> getConnectors();
	GameObject* getPlayer();
};



#endif
