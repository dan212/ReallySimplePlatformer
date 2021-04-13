#include "stdafx.h"
#include "Management.h"


GameLevel::GameLevel(){
	finishFlag = false;
}
GameLevel::~GameLevel(){
	delete player;
	for (int i = connectors.size() - 1; i >= 0; i--){
		delete connectors[i];
	}
	for (int j = blocks.size() - 1; j >= 0; j--){
		delete blocks[j];
	}
}
int GameLevel::buildLevel(std::string filename){
	double minX = 0,
		minY = 0,
		maxX = 0,
		maxY = 0;

	finishFlag = false;

	std::fstream inpfile;
	std::string filePass = "Levels/" + filename;
	inpfile.open(filePass, std::fstream::in);
	struct stat buffer;
	if (!inpfile.good()) {
		return -1;
	}
	else {
		std::string read;
		inpfile >> read;
		levelName = read;
		while (!inpfile.eof()) {
			inpfile >> read;
			double dat_array[9]; // X, Y, h, w, v, x1, y1, x2, y2;
			if (read.find("plr") != -1) {
				for (int i = 0; i < 5; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				player = new Player(dat_array[0], dat_array[1], dat_array[2], dat_array[3], dat_array[4]);
			}
			if (read.find("pla") != -1) {
				for (int i = 0; i < 4; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				blocks.push_back(new SolidBlock(dat_array[0], dat_array[1], dat_array[2], dat_array[3]));
			}
			if (read.find("spk") != -1) {
				for (int i = 0; i < 4; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				blocks.push_back(new TechBlock(dat_array[0], dat_array[1], dat_array[2], dat_array[3], TechTypes::SPIKE));
			}
			if (read.find("mpl") != -1) {
				for (int i = 0; i < 9; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				blocks.push_back(new MovingPlatform(dat_array[0], dat_array[1], dat_array[2], dat_array[3], dat_array[4], dat_array[5], dat_array[6], dat_array[7], dat_array[8]));
			}
			if (read.find("bbl") != -1) {
				for (int i = 0; i < 4; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				blocks.push_back(new BounceBlock(dat_array[0], dat_array[1], dat_array[2], dat_array[3]));
			}
			if (read.find("fin") != -1) {
				for (int i = 0; i < 4; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				blocks.push_back(new TechBlock(dat_array[0], dat_array[1], dat_array[2], dat_array[3], TechTypes::FINISH));
			}
			if (read.find("con") != -1) {
				for (int i = 0; i < 2; i++) {
					inpfile >> read;
					dat_array[i] = stoi(read);
				}
				connectors.push_back(new Connector(blocks[dat_array[0]], blocks[dat_array[1]]));
			}
			if (read.find("gra") != -1) {
				for (int i = 0; i < 2; i++) {
					inpfile >> read;
					dat_array[i] = stod(read);
				}
				gravitation = mathVector2d(dat_array[0], -1 * dat_array[1]); //-1 for testing
			}
			double tmp = dat_array[0];
			if (tmp > maxX){
				maxX = tmp;
			}
			if (tmp < minX){
				minX = tmp;
			}
			tmp = dat_array[1];
			if (tmp > maxY){
				maxY = tmp;
			}
			if (tmp < minY){
				minY = tmp;
			}
		}
		edgeCoords[0] = minX;
		edgeCoords[1] = minY;
		edgeCoords[2] = maxX;
		edgeCoords[3] = maxY;
	}
	return 0;
}
bool GameLevel::isFinished(){
	return finishFlag;
}
mathVector2d GameLevel::getPlayerPos(){
	return player->getPos();
}
std::vector<float> GameLevel::drawLevel(int SWidth, int SHight){
	vector<float> temp_buff;
	vector<float> plr_buff = player->draw(SWidth, SHight);

	for (int i = 0; i < blocks.size(); i++) {
		vector<float> obj_t_buff = blocks[i]->draw(SWidth, SHight);
		for (int j = 0; j < obj_t_buff.size(); j++) {
			temp_buff.push_back(obj_t_buff[j]);
		}
	}
	for (int j = 0; j < plr_buff.size(); j++) {
		temp_buff.push_back(plr_buff[j]);
	}
	return temp_buff;
}
std::vector<float> GameLevel::colourLevel(){
	vector<float> temp_buff;
	vector<float> plr_buff = player->drawColor();
	for (int i = 0; i < blocks.size(); i++) {
		vector<float> obj_t_buff = blocks[i]->drawColor();
		for (int j = 0; j < obj_t_buff.size(); j++) {
			temp_buff.push_back(obj_t_buff[j]);
		}
	}
	for (int j = 0; j < plr_buff.size(); j++) {
		temp_buff.push_back(plr_buff[j]);
	}
	return temp_buff;
}
void GameLevel::updateLevelState(){
	for (int i = 0; i < blocks.size(); i++){
		if (MovingObject* movObj = dynamic_cast <MovingObject*>(blocks[i])){
			movObj->update();
		}
	}
	for (int i = 0; i < connectors.size(); i++){
		connectors[i]->update();
	}
	player->applyForce(gravitation);
	player->update();
	checkCollisions();
}
void GameLevel::applyInputs(){
	if (GetAsyncKeyState(VK_LEFT))
		player->applyForce(mathVector2d(-0.5, 0));
	if (GetAsyncKeyState(VK_RIGHT))
		player->applyForce(mathVector2d(0.5, 0));
	if (GetAsyncKeyState(VK_UP) && !player->getAirFlag()){
		gravitation.y *= -1;
		player->setAirFlag(true);
	}
}
void GameLevel::collide(GameObject* block){
	double vx = (block->getPos().x + block->getWidth() * 0.5) - (player->getPos().x + player->getWidth() * 0.5);
	double vy = (block->getPos().y + block->getHeight() * 0.5) - (player->getPos().y + player->getHeight() * 0.5);

	if (abs(vx) < player->getWidth() * 0.5 + block->getWidth() * 0.5){
		if (abs(vy) < player->getHeight() * 0.5 + block->getHeight() * 0.5){

			bool flag1 = typeid(*block).name() == typeid(SolidBlock).name();
			bool flag2 = typeid(*block).name() == typeid(MovingPlatform).name();
			bool flag3 = typeid(*block).name() == typeid(BounceBlock).name();

			if (flag1 || flag2 || flag3){

				double overlapX = player->getWidth() * 0.5 + block->getWidth() * 0.5 - abs(vx);
				double overlapY = player->getHeight() * 0.5 + block->getHeight() * 0.5 - abs(vy);

				if (overlapY < overlapX){
					if (vy > 0){
						player->setPosY(player->getPos().y - overlapY);
						if (flag3){
							BounceBlock* b = dynamic_cast<BounceBlock *>(block);
							b->hit(player, BlockSides::TOP);
						}
						else {
							player->setAirFlag(false);
							player->applyFriction(dynamic_cast<SolidBlock*>(block)->getFrictionK());
						}
						player->setVelocityY(0);
					}
					else {
						player->setPosY(player->getPos().y + overlapY);
						if (flag3){
							BounceBlock* b = dynamic_cast<BounceBlock *>(block);
							b->hit(player, BlockSides::BOTTOM);
						}
						else {
							player->setAirFlag(false);
							player->applyFriction(dynamic_cast<SolidBlock*>(block)->getFrictionK());
						}
						player->setVelocityY(0);
					}
				}
				else {
					if (vx > 0){
						player->setPosX(player->getPos().x - overlapX);
						if (flag3){
							BounceBlock* b = dynamic_cast<BounceBlock *>(block);
							b->hit(player, BlockSides::LEFT);
						}
						player->setVelocityX(0);
					}
					else {
						player->setPosX(player->getPos().x + overlapX);
						if (flag3){
							BounceBlock* b = dynamic_cast<BounceBlock *>(block);
							b->hit(player, BlockSides::RIGHT);
						}
						player->setVelocityX(0);
					}
				}
			}
			else{
				if (typeid(*block).name() == typeid(TechBlock).name()){
					int block_type = dynamic_cast<TechBlock*>(block)->getType();

					if (block_type == TechTypes::SPIKE){
						player->respawn();
					}
					if (block_type == TechTypes::FINISH){
						finishFlag = true;
					}
				}
			}
		}
	}
}
void GameLevel::checkCollisions(){
	for (int i = 0; i < blocks.size(); i++){
		collide(blocks[i]);
	}
}
std::string GameLevel::getName(){
	return levelName;
}
std::array<double, 4> GameLevel::getEdgeCoords(){
	return edgeCoords;
}
std::vector<GameObject*> GameLevel::getBlocks(){
	return blocks;
}
std::vector<Connector*> GameLevel::getConnectors(){
	return connectors;
}
GameObject * GameLevel::getPlayer(){
	return dynamic_cast<GameObject*>(player);
}