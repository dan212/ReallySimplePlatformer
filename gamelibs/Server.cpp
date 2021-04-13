#include "stdafx.h"
#include "Server.h"

int CALLBACK ConditionAcceptFunc(LPWSABUF lpCallerId,
								 LPWSABUF lpCallerData,
								 LPQOS pQos,
								 LPQOS lpGQOS,
								 LPWSABUF lpCalleeId,
								 LPWSABUF lpCalleeData,
								 GROUP FAR * g,
								 DWORD_PTR dwCallbackData)
{
	int totalClients = dwCallbackData;
	if (totalClients >= 2){
		return CF_REJECT;
	}
	else{
		return CF_ACCEPT;
	}
}

Server::Server(int port){
	serverPort = port;
	numberOfClients = 0;
	exitFlag = false;
}

int Server::activate(){
	char buff[1024];

	if (WSAStartup(0x0202, (WSADATA *)buff))
	{
		return -1;
	}
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		WSACleanup();
		return -1;
	}
	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(serverPort);
	localAddr.sin_addr.s_addr = 0;
	if (bind(serverSocket, (sockaddr *)&localAddr, sizeof(localAddr)))
	{
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}
	if (listen(serverSocket, 256))
	{
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	return 0;
}

void Server::start(){
	testFut = std::async(std::launch::async, &Server::acceptConnections, this);
}

void Server::stop(){
	exitFlag = true;
}

void Server::acceptConnections(){
	int select_res = 0;
	while (!exitFlag){
		while (true)
		{
			if (exitFlag){
				break;
			}
			fd_set s_set;
			FD_ZERO(&s_set);
			FD_SET(serverSocket, &s_set);
			timeval timeout = { 0, 0 };
			select_res = select(serverSocket + 1, &s_set, 0, 0, &timeout);
			if (select_res) break;

			Sleep(250);
		}
		if (!exitFlag){
			sockaddr_in client_addr;
			int client_addr_size = sizeof(client_addr);
			SOCKET client_socket = WSAAccept(serverSocket, (SOCKADDR*)&client_addr, &client_addr_size, &ConditionAcceptFunc, numberOfClients);
			if (client_socket != INVALID_SOCKET){
				numberOfClients++;
				std::async(std::launch::async, &Server::serviceClient, this, client_socket);
			}
		}
	}
}

void Server::serviceClient(SOCKET clientSocket){
	char buff[20 * 1024];
	std::string data = "noData";
	clientsData[clientSocket] = data;

	int bytes_recv;
	while ((bytes_recv = recv(clientSocket, buff, sizeof(buff), 0)) &&
		bytes_recv != SOCKET_ERROR)
	{
		buff[bytes_recv] = '\0';
		data = std::string(buff);
		clientsData[clientSocket] = data;

		for (int i = 0; i < strlen(buff); i++){
			buff[i] = '\0';
		}

		bool dataIsSent = false;
		std::map<SOCKET, std::string>::iterator iter;
		for (iter = clientsData.begin(); iter != clientsData.end(); iter++){
			if (iter->first != clientSocket){
				strcpy(buff, iter->second.c_str());
				send(clientSocket, buff, strlen(buff), 0);
				dataIsSent = true;
			}
		}
		if (!dataIsSent){
			strcpy(buff, "noData");
			send(clientSocket, buff, strlen(buff), 0);
		}

		for (int i = 0; i < strlen(buff); i++){
			buff[i] = '\0';
		}
	}

	numberOfClients--;
	closesocket(clientSocket);
	clientsData.erase(clientSocket);
}