#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <shader.hpp>
#include <Game.h>
#include <SOIL.h>
#include <Interface.h>
//#include <RSPProcess.h>
//using namespace rspp;
class MainProcess;
class GameLevel;

class GraphicsSettings {
public:
	int SWidth, SHight;
	bool FullScreen;

	GraphicsSettings(string Filename);
	GraphicsSettings();
	void ParceSettings(string Filename);
};

class GraphicsHandler {
private:
	GraphicsSettings Settings;
	MainProcess* SourceProc;
	int SWidth, SHight;
	float UVNet[12] = {
		0.0f, 1.0f, //ËÂ
		1.0f, 1.0f, //ÏÂ
		1.0f, 0.0f, //ÏÍ
		0.0f, 1.0f, //ËÂ
		1.0f, 0.0f, //ÏÍ
		0.0f, 0.0f  //ËÍ
	};
	bool isTextured;
	map<string, GLuint> Textures;
	float TopLeftCorX = INT_MAX, TopLeftCorY = INT_MAX, BotRightX = 0, BotRightY = 0;
	int CamType = 1; // 1 - Full Level coverage, 2 - Focus of player
	bool CamTypeSwith = false; // Cheker for current cam type
	float CamFocusMult = 1.0f;

	GLuint programID;
	GLuint programInterID;
	GLuint VertexArrayID;
	GLuint MatrixID;
	GLuint TextureID;

	glm::mat4 Projection;
	glm::mat4 View;
	glm::mat4 Model;
	glm::mat4 MVP;

	GLuint vertexbuffer;
	GLuint colorbuffer;
	GLuint uvbuffer;

	vector<GLfloat> g_color_buffer_data;
	vector<GLfloat> g_vertex_buffer_data;
	vector<string> g_texture_call_data;
	
	GLuint intervertexbuffer;
	GLuint intercolorbuffer;

	vector<GLfloat> inter_color_buffer_data;
	vector<GLfloat> inter_vertex_buffer_data;
	vector<string> inter_texture_call_data;
	vector<GLfloat> inter_UV_buffer_data;
public:
	GLFWwindow* window;
	GraphicsHandler();
	~GraphicsHandler();
	void SetSourcProc(MainProcess* SP);
	void update();
	void changesettings(GraphicsSettings NewSettings);
	vector<GLfloat> CalcVertex(GameLevel* inp);
	vector<GLfloat> CalcColour(GameLevel* inp);
	void AddObjToBuffer(GameLevel * inp);
	void EmptyBuffer();
	int GetWidth() { return SWidth; };
	int GetHight() { return SHight; };
	GLFWwindow* getWindow();
	void UpdateProjection(GameLevel* inp);
	void Flush();
	int preloadTextures();
	void flipTextureSwitch();
	void prepareInterfaceData();
	void prepareButtons();
	void prepareChecks();
	void prepareGlyphs();
	void prepareEdits();
};
#endif