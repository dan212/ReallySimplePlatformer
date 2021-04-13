#include "stdafx.h"
#include "GameSessions.h"


SingleplayerSession::SingleplayerSession(){
	currentLevel = NULL;
}
SingleplayerSession::~SingleplayerSession(){
	deleteLevel();
}
int SingleplayerSession::setLevel(string fileName){
	deleteLevel();
	currentLevel = new GameLevel();
	int err = currentLevel->buildLevel(fileName);
	return err;
}
GameLevel* SingleplayerSession::getLevel(){
	return currentLevel;
}
void SingleplayerSession::deleteLevel(){
	if (currentLevel != NULL){
		delete currentLevel;
	}
	currentLevel = NULL;
}
bool SingleplayerSession::levelFinished(){
	return currentLevel->isFinished();
}
int SingleplayerSession::run(){
	if (currentLevel != NULL){

		currentLevel->updateLevelState();
		currentLevel->applyInputs();
	}
	else {
		return -1;
	}
	return 0;
}

MultiplayerSession::MultiplayerSession(){

	connected = false;
	responseIsReceived = true;
	responseError = false;
	secondPlayerData.player = NULL;
}
MultiplayerSession::~MultiplayerSession(){
	if (secondPlayerData.player != NULL){
		delete secondPlayerData.player;
	}
}
int MultiplayerSession::connectToServer(char servAddr[], int servPort){
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(servPort);
	HOSTENT *hst;

	if (inet_addr(servAddr) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(servAddr);
	else
	{
		if (hst = gethostbyname(servAddr))
			((unsigned long *)&dest_addr.sin_addr)[0] = ((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}

	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		return -2;
	}

	connected = true;
	responseIsReceived = true;
	responseError = false;

	return 0;
}
void MultiplayerSession::requestToServer(){
	responseError = false;
	char buff[1024];
	if (!connected) return;
	strcpy(buff,formRequest().c_str());
	send(my_sock, &buff[0], strlen(&buff[0]), 0);
	for (int i = 0; i < strlen(buff); i++){
		buff[i] = '\0';
	}

	int nsize;
	if ((nsize = recv(my_sock, &buff[0], sizeof(buff)-1, 0)) != SOCKET_ERROR){
		buff[nsize] = '\0';
		parseResponse(buff);
	}
	else{
		closesocket(my_sock);
		WSACleanup();
		responseError = true;
	}
	responseIsReceived = true;
}
std::string MultiplayerSession::formRequest(){
	return (currentLevel->getName() + " " +
			std::to_string(currentLevel->getPlayerPos().x) + " " +
			std::to_string(currentLevel->getPlayerPos().y)
			);
}
void MultiplayerSession::parseResponse(char *response){
	float x, y;
	char s[100];
	
	if (strcmp(response, "noData") != 0){
		printf("STROKA: %s \n", response);
		sscanf(response, "%s %f %f", s, &x, &y);
		printf("DATA: %s %f %f \n", s, x, y);
		if (secondPlayerData.player == NULL){
			double w = currentLevel->getPlayer()->getWidth();
			double h = currentLevel->getPlayer()->getHeight();
			secondPlayerData.player = new Player(x, y, w, h, 1);
			secondPlayerData.levelName = s;
		}
		else{
			secondPlayerData.player->setPos(mathVector2d(x, y));
			secondPlayerData.levelName = s;
		}
	}
	else{
		if (secondPlayerData.player != NULL){
			delete secondPlayerData.player;
			secondPlayerData.player = NULL;
			secondPlayerData.levelName.clear();
		}
	}
}
int MultiplayerSession::run(){
	if (currentLevel != NULL){

		if (responseIsReceived){
			responseIsReceived = false;
			if (responseError){
				return -2;
			}
			else{
				asyFut = std::async(std::launch::async, &MultiplayerSession::requestToServer, this);
			}
		}

		currentLevel->updateLevelState();
		currentLevel->applyInputs();
	}
	else {
		return -1;
	}
	return 0;
}
int MultiplayerSession::activate(){
	char buff[1024];
	if (WSAStartup(0x202, (WSADATA *)&buff[0])){
		return -1;
	}
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0){
		return -1;
	}
	return 0;
}
void MultiplayerSession::disconnect(){
	connected = false;
	closesocket(my_sock);
	WSACleanup();
}
playerData MultiplayerSession::getData(){
	return secondPlayerData;
}