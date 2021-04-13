#include "Graphics.h"
#include "RSPProcess.h"
#include "Management.h"

GraphicsHandler::GraphicsHandler()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		//return -1;
		exit(1);
	}

	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	Settings.ParceSettings("Settings.txt");
	window = glfwCreateWindow(Settings.SWidth, Settings.SHight, "Really Simple Platformer", NULL, NULL);
	SWidth = Settings.SWidth;
	SHight = Settings.SHight;
	isTextured = false;
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		system("PAUSE");
		exit(1);
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		system("PAUSE");
		exit(1);
	}

	if (preloadTextures() == -1) {
		fprintf(stderr, "Failed to load textures\n");
		getchar();
		glfwTerminate();
		system("PAUSE");
		exit(1);
	}
	
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	programInterID = LoadShaders("InterfaceShader.vertexshader", "InterfaceShader.fragmentshader");
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	MatrixID = glGetUniformLocation(programID, "MVP");
	Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0.5f, 0.5f, 1), // Camera is at (0,0,-1), in World Space
		glm::vec3(0.5f, 0.5f, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upsideown)
	);
	Model = glm::mat4(1.0f);
	MVP = Projection * View * Model;
	glGenBuffers(1, &colorbuffer);
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &intercolorbuffer);
	glGenBuffers(1, &intervertexbuffer);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UVNet), &UVNet[0], GL_STATIC_DRAW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GraphicsHandler::~GraphicsHandler()
{
	glDeleteBuffers(1,&uvbuffer);
	glDeleteBuffers(1,&vertexbuffer);
	glDeleteBuffers(1,&colorbuffer);
	glDeleteBuffers(1,&intercolorbuffer);
	glDeleteBuffers(1,&intervertexbuffer);
	glfwTerminate();
}


void GraphicsHandler::SetSourcProc(MainProcess* SP)
{
	SourceProc = SP;
}

void GraphicsHandler::update()
{
	EmptyBuffer();

	//Backdrop render

	vector<GLfloat> backdrop_vert = {
	-1.0f,1.0f,0.3f,
	1.0f,1.0f,0.3f,
	1.0f,-1.0f,0.3f,
	-1.0f,1.0f,0.3f,
	1.0f,-1.0f,0.3f,
	-1.0f,-1.0f,0.3f,
	};


	if (SourceProc->inGame) {
		if (SourceProc->Single) {
			vector<GLfloat> temp_vert_buff = SourceProc->SPSession->getLevel()->drawLevel(SWidth, SHight);
			vector<GLfloat> temp_colour_buff = SourceProc->SPSession->getLevel()->colourLevel();
			for (int i = 0; i < temp_vert_buff.size(); i++) {
				g_vertex_buffer_data.push_back(temp_vert_buff[i]);
			}
			if (isTextured) {
				for (int i = 0; i < SourceProc->SPSession->getLevel()->getBlocks().size(); i++) {
					g_texture_call_data.push_back(SourceProc->SPSession->getLevel()->getBlocks()[i]->getClassName());
					if (g_texture_call_data[i] == "TechBlock") {
						if (dynamic_cast<TechBlock*>(SourceProc->SPSession->getLevel()->getBlocks()[i])->getType() == 0) {
							g_texture_call_data[i] = "Spike";
						}
						else {
							g_texture_call_data[i] = "Finish";
						}
					}
				}
				g_texture_call_data.push_back("Player");
			}
			else {
				for (int i = 0; i < temp_colour_buff.size(); i++) {
					g_color_buffer_data.push_back(temp_colour_buff[i]);
				}
			}
		}
		else {
			vector<GLfloat> temp_vert_buff = SourceProc->MPSession->getLevel()->drawLevel(SWidth, SHight);
			vector<GLfloat> temp_colour_buff = SourceProc->MPSession->getLevel()->colourLevel();
			for (int i = 0; i < temp_vert_buff.size(); i++) {
				g_vertex_buffer_data.push_back(temp_vert_buff[i]);
			}
			if (isTextured) {
				for (int i = 0; i < SourceProc->MPSession->getLevel()->getBlocks().size(); i++) {
					g_texture_call_data.push_back(SourceProc->MPSession->getLevel()->getBlocks()[i]->getClassName());
					if (g_texture_call_data[i] == "TechBlock") {
						if (dynamic_cast<TechBlock*>(SourceProc->MPSession->getLevel()->getBlocks()[i])->getType() == 0) {
							g_texture_call_data[i] = "Spike";
						}
						else {
							g_texture_call_data[i] = "Finish";
						}
					}
				}
				g_texture_call_data.push_back("Player");
				if (SourceProc->MPSession->getData().player != NULL) {
					if (SourceProc->MPSession->getData().levelName == SourceProc->MPSession->getLevel()->getName()) {
						//Пушнуть в g_vertex_buffer_data коорды 2го игрока
						temp_vert_buff = SourceProc->MPSession->getData().player->draw(SWidth,SHight);
						for (int i = 0; i < temp_vert_buff.size(); i++) {
							g_vertex_buffer_data.push_back(temp_vert_buff[i]);
						}
						g_texture_call_data.push_back("Player");
					}
				}
			}
			else {
				for (int i = 0; i < temp_colour_buff.size(); i++) {
					g_color_buffer_data.push_back(temp_colour_buff[i]);
				}
			}
			//vector<GLfloat> temp_vert_buff = SourceProc->MPSession->getLevel()->drawLevel(SWidth, SHight);
			//vector<GLfloat> temp_colour_buff = SourceProc->MPSession->getLevel()->colourLevel();
			//for (int i = 0; i < temp_vert_buff.size() / 18; i++) {
			//	g_vertex_buffer_data.push_back(temp_vert_buff[i]);
			//	if (isTextured) {
			//		g_texture_call_data.push_back(SourceProc->MPSession->getLevel()->getBlocks()[i]->getClassName());
			//		if (g_texture_call_data[i] == "TechBlock") {
			//			if (dynamic_cast<TechBlock*>(SourceProc->MPSession->getLevel()->getBlocks()[i])->getType() == 0) {
			//				g_texture_call_data[i] = "Spike";
			//			}
			//			else {
			//				g_texture_call_data[i] = "Finish";
			//			}
			//		}
			//		g_texture_call_data.push_back("Player");
			//		//if (SourceProc->MPSession->getSecondPlayerDat() != NULL) {
			//		//	if (SourceProc->MPSession->getSecondPlayerDat().levelName == SourceProc->MPSession->getLevel()->getName()) {
			//		//		Пушнуть в g_vertex_buffer_data коорды 2го игрока
			//		//		g_texture_call_data.push_back("Player");
			//		//	}
			//		//}
			//	}
			//	else {
			//		g_color_buffer_data.push_back(temp_colour_buff[i]);
			//	}
			//}
		}
	}
	if (!SourceProc->Interface->getHiden()) {
		prepareInterfaceData();
	}
	//TODO - Дописать бэкдроп
	//for (int i = 0; i < backdrop_vert.size(); i++) {
	//	g_vertex_buffer_data.push_back(backdrop_vert[i]);
	//}
	//g_texture_call_data.push_back("Backdrop");
	// Отрисовка объектов
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	if (g_vertex_buffer_data.size() != 0) {
		glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);
	}
	
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(programID);

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	if (isTextured) {
		for (int i = 0; i < g_vertex_buffer_data.size() / 18; i++) {
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), &g_vertex_buffer_data[i*18], GL_STATIC_DRAW);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Textures[g_texture_call_data[i]]);
			glUniform1i(TextureID, 0);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(UVNet), UVNet, GL_STATIC_DRAW);
			glVertexAttribPointer(
				1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
				2,                                // size : U+V => 2
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);

			glDrawArrays(GL_TRIANGLES, 0, 6);
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
		if (!SourceProc->Interface->getHiden()) {
			for (int i = 0; i < inter_vertex_buffer_data.size() / 18; i++) {
				// Отрисовка интерфейса
				glBindBuffer(GL_ARRAY_BUFFER, intervertexbuffer);
				glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), &inter_vertex_buffer_data[i * 18], GL_STATIC_DRAW);

				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, intervertexbuffer);
				glVertexAttribPointer(
					0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Textures[inter_texture_call_data[i]]);
				glUniform1i(TextureID, 0);
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
				glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), &inter_UV_buffer_data[i*12], GL_STATIC_DRAW);
				glVertexAttribPointer(
					1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
					2,                                // size : U+V => 2
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);
				glDrawArrays(GL_TRIANGLES, 0,6);
				
			}
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
	}
	else {
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, g_vertex_buffer_data.size() * sizeof(GLfloat), &g_vertex_buffer_data[0], GL_STATIC_DRAW);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, g_color_buffer_data.size() * sizeof(GLfloat), &g_color_buffer_data[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, g_vertex_buffer_data.size());
		// Отрисовка объектов - done
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		if ((!SourceProc->Interface->getHiden())&&(!inter_vertex_buffer_data.empty())) {
			// Отрисовка интерфейса
			glBindBuffer(GL_ARRAY_BUFFER, intervertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, inter_vertex_buffer_data.size() * sizeof(GLfloat), &inter_vertex_buffer_data[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, intercolorbuffer);
			glBufferData(GL_ARRAY_BUFFER, inter_color_buffer_data.size() * sizeof(GLfloat), &inter_color_buffer_data[0], GL_STATIC_DRAW);

			glUseProgram(programInterID);

			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, intervertexbuffer);
			glVertexAttribPointer(
				0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, intercolorbuffer);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

			//glDrawArrays(GL_TRIANGLES, 0, (3 * (SourceProc->Interface->interObjects.size()) * 2));
			glDrawArrays(GL_TRIANGLES, 0, inter_vertex_buffer_data.size());
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
	}
	// Отрисовка интерфейса - done
	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void GraphicsHandler::changesettings(GraphicsSettings NewSettings)
{
	glfwSetWindowSize(window, NewSettings.SWidth, NewSettings.SHight);
	SWidth = NewSettings.SWidth;
	SHight = NewSettings.SHight;
}

vector<GLfloat> GraphicsHandler::CalcVertex(GameLevel * inp)
{
	return inp->drawLevel(SWidth, SHight);
}

vector<GLfloat> GraphicsHandler::CalcColour(GameLevel* inp) {
	return inp->colourLevel();
	
}

void GraphicsHandler::AddObjToBuffer(GameLevel* inp)
{
	vector<GLfloat> temp_vert_buf = CalcVertex(inp);
	vector<GLfloat> temp_col_buf = CalcColour(inp);
	for (int j = 0; j < temp_vert_buf.size(); j++) {
		g_vertex_buffer_data.push_back(temp_vert_buf[j]);
		g_color_buffer_data.push_back(temp_col_buf[j]);
	}
}

void GraphicsHandler::EmptyBuffer()
{
	g_vertex_buffer_data.clear();
	g_color_buffer_data.clear();
	inter_color_buffer_data.clear();
	inter_vertex_buffer_data.clear();
	g_texture_call_data.clear();
	inter_texture_call_data.clear();
	inter_UV_buffer_data.clear();
}

GLFWwindow * GraphicsHandler::getWindow()
{
	return window;
}

void GraphicsHandler::UpdateProjection(GameLevel * inp)
{
//	float *posarr = inp->getEdgeCoords(); // posarr = {minX,minY,maxX,maxY}
	array<double,4> posarr = inp->getEdgeCoords();
	float minX = posarr[0] , minY = posarr[1], maxX = posarr[2], maxY = posarr[3];
	float deltaX = (abs(minX) + maxX)/SWidth, deltaY = (abs(minY) + maxY)/SHight;
	Projection = glm::ortho(-deltaX/2,deltaX/2,-deltaY/2,deltaY/2,0.0f,50.0f);
	//Projection = glm::ortho(-2.0f, 2.0f,-2.0f,2.0f,0.0f,50.0f);
	MVP = Projection * View * Model;
}

void GraphicsHandler::Flush()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glGenBuffers(1, &colorbuffer);
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &intercolorbuffer);
	glGenBuffers(1, &intervertexbuffer);
	glGenBuffers(1, &uvbuffer);
}

int GraphicsHandler::preloadTextures()
{
	printf("Loading textures... \n");
	
	Textures.insert(make_pair("SolidBlock",SOIL_load_OGL_texture(
		"SolidBlock.png",
		SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["SolidBlock"] == 0)
	{
		printf("SOIL Loading error on file SolidBlock.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("SolidBlock.png \n");
	}

	Textures.insert(make_pair("MovingPlatform", SOIL_load_OGL_texture(
		"MovingPlatform.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["MovingPlatform"] == 0)
	{
		printf("SOIL Loading error on file MovingPlatform.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("MovingObject.png \n");
	}

	Textures.insert(make_pair("Spike", SOIL_load_OGL_texture(
		"Spike.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["Spike"] == 0)
	{
		printf("SOIL Loading error on file Spike.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("Spike.png \n");
	}


	Textures.insert(make_pair("Finish", SOIL_load_OGL_texture(
		"Finish.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["Finish"] == 0)
	{
		printf("SOIL Loading error on file Finish.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("Finish.png \n");
	}

	Textures.insert(make_pair("BounceBlock",SOIL_load_OGL_texture(
		"BounceBlock.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["BounceBlock"] == 0)
	{
		printf("SOIL Loading error on file BounceBlock.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("BounceBlock.png \n");
	}

	Textures.insert(make_pair("Font",SOIL_load_OGL_texture(
		"Font.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["Font"] == 0)
	{
		printf("SOIL Loading error on file Font.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("Font.png \n");
	}

	Textures.insert(make_pair("Player",SOIL_load_OGL_texture(
		"Player.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["Player"] == 0)
	{
		printf("SOIL Loading error on file Player.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("Player.png \n");
	}

	Textures.insert(make_pair("InterfaceButton",SOIL_load_OGL_texture(
		"InterfaceButton.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["InterfaceButton"] == 0)
	{
		printf("SOIL Loading error on file InterfaceButton.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("InterfaceButton.png \n");
	}

	Textures.insert(make_pair("InterfaceObject", SOIL_load_OGL_texture(
		"InterfaceObject.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["InterfaceObject"] == 0)
	{
		printf("SOIL Loading error on file InterfaceObject.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("InterfaceObject.png \n");
	}

	Textures.insert(make_pair("Backdrop", SOIL_load_OGL_texture(
		"Backdrop.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT
	)));
	if (Textures["Backdrop"] == 0)
	{
		printf("SOIL Loading error on file Backdrop.png: %s \n", SOIL_last_result);
		return -1;
	}
	else {
		printf("Backdrop.png \n");
	}
	printf("Texture loading sucsessful \n", SOIL_last_result);
	return 1;
}

void GraphicsHandler::flipTextureSwitch()
{
	isTextured = !isTextured;
	if (isTextured) {
		programID = LoadShaders("TextureVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	}
	else {
		programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");
	}
	TextureID = glGetUniformLocation(programID, "myTextureSampler");
	glGenBuffers(1, &colorbuffer);
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &intercolorbuffer);
	glGenBuffers(1, &intervertexbuffer);
	glGenBuffers(1, &uvbuffer);
}

void GraphicsHandler::prepareInterfaceData()
{
	InterfaceHandler* curInter = SourceProc->Interface;
	if (curInter->menu->getMenuCount() != 0) {
		if (curInter->menu->getCurrentMenu()->getMenuSize() != 0) {
			//Prep Buttons
			prepareButtons();
			//Prep Checkboxes
			prepareChecks();
			//Prep Glyphs
			prepareGlyphs();
			//Prep Edits 
			prepareEdits();
		}
	}
}

void GraphicsHandler::prepareButtons()
{
	map<string, InterfaceButton*>::iterator btn;
	vector<InterfaceGlyph>::iterator gl;
	map<string, InterfaceButton*> btnMap = SourceProc->Interface->menu->getCurrentMenu()->getButtonsMap();
	for (btn = btnMap.begin(); btn != btnMap.end(); btn++) {
		std::vector<GLfloat> temp_vect = btn->second->draw(SWidth, SHight);
		for (int i = 0; i < temp_vect.size(); i++) {
			inter_vertex_buffer_data.push_back(temp_vect[i]);
		}
		if (isTextured) {
			inter_texture_call_data.push_back("InterfaceButton");
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(UVNet[i]);
			}
		}
		else {
			temp_vect = btn->second->color();
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_color_buffer_data.push_back(temp_vect[i]);
			}
		}
		vector<InterfaceGlyph> tempVect = btn->second->getLable()->getGlyphs();
		for (gl = tempVect.begin(); gl != tempVect.end(); gl++) {
			std::vector<GLfloat> temp_vect = gl->draw(SWidth, SHight);
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_vertex_buffer_data.push_back(temp_vect[i]);
			}
			inter_texture_call_data.push_back("Font");
			float* tempUV = gl->getUVCords();
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(tempUV[i]);
			}
		}
	}
}

void GraphicsHandler::prepareChecks()
{
	map<string, InterfaceCheckbox*>::iterator ch;
	map<string, InterfaceCheckbox*> chMap = SourceProc->Interface->menu->getCurrentMenu()->getMenuCheckboxes();
	for (ch = chMap.begin(); ch != chMap.end(); ch++) {
		std::vector<GLfloat> temp_vect = ch->second->draw(SWidth, SHight);
		for (int i = 0; i < temp_vect.size(); i++) {
			inter_vertex_buffer_data.push_back(temp_vect[i]);
		}
		if (isTextured) {
			inter_texture_call_data.push_back("InterfaceObject");
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(UVNet[i]);
			}
		}
		else {
			temp_vect = ch->second->color();
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_color_buffer_data.push_back(temp_vect[i]);
			}
		}
		temp_vect = ch->second->getGlyph()->draw(SWidth, SHight);
		for (int i = 0; i < temp_vect.size(); i++) {
			inter_vertex_buffer_data.push_back(temp_vect[i]);
		}
		inter_texture_call_data.push_back("Font");
		float* tempUV = ch->second->getGlyph()->getUVCords();
		for (int i = 0; i < 12; i++) {
			inter_UV_buffer_data.push_back(tempUV[i]);
		}
	}
}

void GraphicsHandler::prepareGlyphs()
{
	map < string, InterfaceLable*> ::iterator lab;
	vector<InterfaceGlyph>::iterator gl;
	map<string, InterfaceLable*> labMap = SourceProc->Interface->menu->getCurrentMenu()->getMenuLables();
	for (lab = labMap.begin(); lab != labMap.end(); lab++) {
		vector<InterfaceGlyph> tempVect = lab->second->getGlyphs();
		for (gl = tempVect.begin(); gl != tempVect.end(); gl++) {
			std::vector<GLfloat> temp_vect = gl->draw(SWidth, SHight);
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_vertex_buffer_data.push_back(temp_vect[i]);
			}
			inter_texture_call_data.push_back("Font");
			float* tempUV = gl->getUVCords();
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(tempUV[i]);
			}
		}
	}
}

void GraphicsHandler::prepareEdits()
{
	map<string, InterfaceTextEdit*>::iterator te;
	vector<InterfaceGlyph>::iterator gl;
	map<string, InterfaceTextEdit*> teMap = SourceProc->Interface->menu->getCurrentMenu()->getMenuTextEdits();
	for (te = teMap.begin(); te != teMap.end(); te++) {
		std::vector<GLfloat> temp_vect = te->second->draw(SWidth, SHight);
		for (int i = 0; i < temp_vect.size(); i++) {
			inter_vertex_buffer_data.push_back(temp_vect[i]);
		}
		if (isTextured) {
			inter_texture_call_data.push_back("InterfaceObject");
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(UVNet[i]);
			}
		}
		else {
			temp_vect = te->second->color();
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_color_buffer_data.push_back(temp_vect[i]);
			}
		}
		vector<InterfaceGlyph> tempVect = te->second->getLable()->getGlyphs();
		for (gl = tempVect.begin(); gl != tempVect.end(); gl++) {
			std::vector<GLfloat> temp_vect = gl->draw(SWidth, SHight);
			for (int i = 0; i < temp_vect.size(); i++) {
				inter_vertex_buffer_data.push_back(temp_vect[i]);
			}
			inter_texture_call_data.push_back("Font");
			float* tempUV = gl->getUVCords();
			for (int i = 0; i < 12; i++) {
				inter_UV_buffer_data.push_back(tempUV[i]);
			}
		}
	}
}

GraphicsSettings::GraphicsSettings(string Filename)
{
	ParceSettings(Filename);
}

GraphicsSettings::GraphicsSettings()
{
	ParceSettings("Settings.txt"); //Trying to use defaul settings file.
}

void GraphicsSettings::ParceSettings(string Filename)
{
	fstream settingsfile;
	settingsfile.open(Filename, fstream::in);
	if (settingsfile.good()) {
		string read;
		while (!settingsfile.eof()) {
			settingsfile >> read;
			if (read.find("Width") != -1) {
				settingsfile >> read;
				SWidth = stoi(read);
			}
			if (read.find("Hight") != -1) {
				settingsfile >> read;
				SHight = stoi(read);
			}
			if (read.find("FullS") != -1) {
				settingsfile >> read;
				FullScreen = (stoi(read) == 1);
			}
		}
		settingsfile.close();
	}
	else {
		//Settings file not found, aplying default ones
		SWidth = 800;
		SHight = 600;
		FullScreen = false;
		settingsfile.close();
		//Creating new settings file, with default settings
		settingsfile.open(Filename, fstream::out);
		settingsfile << "Width " << SWidth << endl;
		settingsfile << "Hight " << SHight << endl;
		settingsfile << "FullS " << "0" << endl;
	}
}
