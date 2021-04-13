#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <RSPProcess.h>

int main(void)
{
	MainProcess* Game = new MainProcess();
	Game->Graphics->SetSourcProc(Game);
	while (!GetAsyncKeyState(VK_ESCAPE)) {
		Game->update();
	}
	delete Game;
	return 0;
}
