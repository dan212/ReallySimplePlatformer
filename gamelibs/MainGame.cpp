// MainGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameSessions.h"
#include "Server.h"

#include <iostream>
using namespace std;

void main()
{
	string lvlFile = "level2.txt";

	Server gameServ(6666);
	if (!gameServ.activate()){
		gameServ.start();

		MultiplayerSession game;
		int actErr = game.activate();
		if (!game.setLevel(lvlFile)){
			int code = game.connectToServer("127.0.0.1", 6666);
			if (code == 0){
				while (!GetAsyncKeyState(VK_ESCAPE)){
					int err = game.run();
					if (err == -1) {
						cout << "Level isn't set" << endl;
						break;
					}
					if (err == -2) {
						cout << "Connection error" << endl;
						break;
					}
				}
			}
			else{
				if (code == -1) cout << "Invalid addres" << endl;
				if (code == -2) cout << "Connection error" << endl;
				if (code == -3) cout << "Socket() error" << endl;
			}
		}

		gameServ.stop();
	}
	system("PAUSE");
}
