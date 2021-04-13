#ifndef _GAMESESSIONS_H_
#define _GAMESESSIONS_H_

#include "SocketLibs.h"
#include "Management.h"

class SingleplayerSession{
protected:
	GameLevel *currentLevel;
public:
	SingleplayerSession();
	~SingleplayerSession();
	int setLevel(string fileName);
	GameLevel* getLevel();
	void deleteLevel();
	bool levelFinished();
	virtual int run();
};

struct playerData{
	Player *player;
	std::string levelName;
};

class MultiplayerSession : public SingleplayerSession{
private:
	SOCKET my_sock;

	bool responseIsReceived;
	bool responseError;

	bool connected;

	playerData secondPlayerData;
	std::future<void> asyFut;

	void requestToServer();
	std::string formRequest();
	void parseResponse(char *response);
public:
	MultiplayerSession();
	~MultiplayerSession();
	int connectToServer(char servAddr[], int servPort);
	void disconnect();
	int run();
	int activate();

	playerData getData();
};


#endif
