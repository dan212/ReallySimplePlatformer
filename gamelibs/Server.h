#ifndef _SERVER_
#define _SERVER_

#include <map>
#include <string>
#include "SocketLibs.h"

class Server{
private:
	int serverPort;
	int numberOfClients;
	bool exitFlag;

	std::future<void> testFut;
	SOCKET serverSocket;
	std::map <SOCKET, std::string> clientsData;

	void acceptConnections();
	void serviceClient(SOCKET clientSocket);
public:
	Server(int port);
	int activate();
	void start();
	void stop();
};

int CALLBACK ConditionAcceptFunc(LPWSABUF lpCallerId,
	LPWSABUF lpCallerData,
	LPQOS pQos,
	LPQOS lpGQOS,
	LPWSABUF lpCalleeId,
	LPWSABUF lpCalleeData,
	GROUP FAR * g,
	DWORD_PTR dwCallbackData);


#endif