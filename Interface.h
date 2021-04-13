#pragma once

#include <Graphics.h>
//#include <Windows.h>
#include <functional>
#include <iostream>
#include <map>
//#include <string>
//#include <vector>
int limitInt(int inp, int Up, int Down);
struct position {
	double X, Y;
};
class InterfaceHandler;
class GraphicsHandler;
class InterfaceObject {
protected:
	int colour[4]; //R,G,B,A
	float briteMult = 1.0f;
	position pos;
	double width, height;
	bool isClickable = false;
	bool isClicked = false;
	bool isHovered = false;
public:
	InterfaceObject() {};
	InterfaceObject(int posX, int posY, int h, int w);
	InterfaceHandler* sourceInterface;
	position GetPos();
	double GetWidth();
	double GetHeigth();
	void setPos(int PosX, int PosY);
	void setSize(int h, int w);
	void bindInterface(InterfaceHandler* inp);
	void SetColour(int Red, int Green, int Blue, int Alpha);
	void update();
	vector<float> draw(int SWidth, int SHight);
	vector<float> color();
};

class InterfaceGlyph : public InterfaceObject {
private:
	char symbol;
	float UVCords[12];
public:
	InterfaceGlyph(int posX, int posY, int size, char sym);
	void setSymb(char sym);
	void calcSize(int size);
	void calcUVCords();
	float* getUVCords();
};

class InterfaceLable : public InterfaceObject {
private:
	string text;
	vector<InterfaceGlyph> glyphs;
	int textsize;
public:
	InterfaceLable(int posX, int posY, int size, string inptext);
	void SetText(string inptext);
	void figureGlyphs();
	void setSize(int size);
	string getText() { return text; };
	vector<InterfaceGlyph> getGlyphs() { return glyphs; };
};

class InterfaceButton : public InterfaceObject {
private:
	function<void()> action;
	InterfaceLable *buttonLable;
	bool wasClicked;
public:
	InterfaceButton(int posX, int posY, int h, int w);
	InterfaceButton(int posX, int posY, int h, int w, function<void()> inpf);
	InterfaceButton(int posX, int posY, int h, int w, string btnName, function<void()> inpf);
	void SetAction(function<void()> inpf);
	void DoAction();
	void update();
	void setPos(int PosX, int PosY);
	void setSize(int h, int w);
	InterfaceLable* getLable() { return buttonLable; };
};

class InterfaceCheckbox : public InterfaceButton {
private:
	bool check = false;
	InterfaceGlyph *checkLable;
	void f_flip();
public:
	InterfaceCheckbox(int posX, int posY, int h, int w);
	bool isChecked();
	void DoAction();
	InterfaceGlyph* getGlyph() { return checkLable; };
};

class InterfaceTextEdit : public InterfaceObject {
private:
	InterfaceLable* curStr;
	int maxChar;
	double lastInpT;
	bool hadInp;
public:
	InterfaceTextEdit(int posX, int posY, int h, int w);
	string getString();
	InterfaceLable* getLable();
	void setText(string inpT);
	void resolveInp();
	void update();
};

class mouseCursor {
private:
	InterfaceHandler* sourceInterface;
	position pos;
	GLFWcursor* GCurs = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	bool Click = false;
	bool Pressed = false;
	InterfaceObject* HoveredObj;
	InterfaceButton* HoveredButton;
	function<void()> action;
	bool actionArmed = false;
public:
	void SetAction(function<void()> inpf);
	void dropAction();
	void update();
	InterfaceObject* GetHoveredObj();
	InterfaceButton* GetHoveredBtn();
	bool getClick();
	void bindInterface(InterfaceHandler* inter);
};

class InterfaceMenu {
private:
	string menuName;
	map<string,InterfaceButton*> menuButtons;
	map<string,InterfaceLable*> menuLables;
	map<string,InterfaceCheckbox*> menuCheckboxes;
	map<string, InterfaceTextEdit*> menuTextEdits;
	InterfaceHandler* sourceInterface;
public:
	InterfaceMenu(string name);
	void bindInter(InterfaceHandler* inpInter);
	string getName();
	void addButton(string btnName,InterfaceButton *inpB);
	void addLable(string labName,InterfaceLable *inpL);
	void addCheckbox(string chName, InterfaceCheckbox *inpCh);
	void addTextEdit(string TEName, InterfaceTextEdit *inpTE);
	map<string, InterfaceButton*> getButtonsMap() { return menuButtons; };
	map<string, InterfaceLable*> getMenuLables() { return menuLables; };
	map<string, InterfaceCheckbox*> getMenuCheckboxes() { return menuCheckboxes; };
	map<string, InterfaceTextEdit*> getMenuTextEdits() { return menuTextEdits; };
	void bindActionToBtn(string btnName, function<void ()> inpF);
	bool getCheckboxState(string checkName);
	string getLableContent(string lableName);
	int getMenuSize();
	void update();
};

class MenuPackager {
private:
	InterfaceHandler* sourceInterface;
	map<string, InterfaceMenu*> menus;
	string activeMenuName;
public:
	MenuPackager(InterfaceHandler* sorcI);
	void createMenu(string menuName);
	void switchToMenu(string newMenu) { activeMenuName = newMenu; };
	InterfaceMenu* getMenuByName(string menuName);
	InterfaceMenu* getCurrentMenu();
	map<string, InterfaceMenu*> getMenus() { return menus; };
	int getMenuCount();
	void update();
};

class InterfaceHandler {
private:
	bool isBound;
	bool isHiden;
public:
	InterfaceHandler(GraphicsHandler* UsedGraphics);
	GraphicsHandler* Graphics;
	mouseCursor* Cur;
	MenuPackager* menu;
	void bindGraphicsObj(GraphicsHandler* inp);
	void hide();
	void unhide();
	bool getHiden() { return isHiden; };
	void createLable(int posX, int posY, int s, string inpT);
	void createButton(string btnName,int posX, int posY, int h, int w, function<void()> inpf);
	void update();
};