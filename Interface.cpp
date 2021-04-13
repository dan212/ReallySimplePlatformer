#include "Interface.h"
#include "Windows.h"

void mouseCursor::SetAction(function<void()> inpf)
{
	action = inpf;
	actionArmed = true;
}

void mouseCursor::dropAction()
{
	action = NULL;
	actionArmed = false;
}

void mouseCursor::update()
{
	glfwGetCursorPos(sourceInterface->Graphics->getWindow(), &pos.X, &pos.Y);
	//pos.X += sourceInterface->Graphics->GetWidth() / 2;
	//pos.Y += sourceInterface->Graphics->GetHight() / 2;
	if (GetAsyncKeyState(VK_LBUTTON)) {
		Pressed = true;
	}
	if (GetAsyncKeyState(VK_LBUTTON) && Pressed) {
		Pressed = false;
		Click = true;
	}
	else { Click = false; }
	HoveredObj = NULL;
	HoveredButton = NULL;
	if (!sourceInterface->getHiden()) {
		map<string, InterfaceButton*> tempmapB = sourceInterface->menu->getCurrentMenu()->getButtonsMap();
		for (auto intB = tempmapB.begin(); intB != tempmapB.end(); intB++) {
				position tpos = intB->second->GetPos();
				double tw = intB->second->GetWidth();
				double th = intB->second->GetHeigth();
				if (pos.X > tpos.X && pos.X < tpos.X + tw && pos.Y > tpos.Y && pos.Y < tpos.Y + th) {
					HoveredButton = intB->second;
				}
		}
		map<string, InterfaceCheckbox*> tempmapC = sourceInterface->menu->getCurrentMenu()->getMenuCheckboxes();
		for (auto intB = tempmapC.begin(); intB != tempmapC.end(); intB++) {
			position tpos = intB->second->GetPos();
			double tw = intB->second->GetWidth();
			double th = intB->second->GetHeigth();
			if (pos.X > tpos.X && pos.X < tpos.X + tw && pos.Y > tpos.Y && pos.Y < tpos.Y + th) {
				HoveredButton = intB->second;
			}
		}
		map<string, InterfaceTextEdit*> tempmapTE = sourceInterface->menu->getCurrentMenu()->getMenuTextEdits();
		for (auto intB = tempmapTE.begin(); intB != tempmapTE.end(); intB++) {
			position tpos = intB->second->GetPos();
			double tw = intB->second->GetWidth();
			double th = intB->second->GetHeigth();
			if (pos.X > tpos.X && pos.X < tpos.X + tw && pos.Y > tpos.Y && pos.Y < tpos.Y + th) {
				HoveredObj = intB->second;
			}
		}
	}
}

InterfaceObject * mouseCursor::GetHoveredObj()
{
	return HoveredObj;
}

InterfaceButton * mouseCursor::GetHoveredBtn()
{
	return HoveredButton;
}

bool mouseCursor::getClick()
{
	return Click;
}

void mouseCursor::bindInterface(InterfaceHandler* inter)
{
	sourceInterface = inter;
	//GCurs = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	glfwSetCursor(sourceInterface->Graphics->window, GCurs);
}


InterfaceHandler::InterfaceHandler(GraphicsHandler * UsedGraphics)
{
	Graphics = UsedGraphics;
	isBound = false;
	isHiden = true;
	Cur = new mouseCursor();
	Cur->bindInterface(this);
	menu = new MenuPackager(this);
}

void InterfaceHandler::bindGraphicsObj(GraphicsHandler* inp)
{
	Graphics = inp;
	isBound = true;
}

void InterfaceHandler::hide()
{
	isHiden = true;
}

void InterfaceHandler::unhide()
{
	isHiden = false;
}

void InterfaceHandler::createLable(int posX, int posY, int s, string inpT)
{
	InterfaceLable *newobj = new InterfaceLable(posX, posY, s, inpT);
	newobj->bindInterface(this);
	newobj->SetColour(rand() % 255, rand() % 255, rand() % 255, 150);
	string t_str = itoa(menu->getCurrentMenu()->getMenuLables().size(),new char(),10);
	menu->getCurrentMenu()->getMenuLables().insert(make_pair(t_str, newobj));
}

void InterfaceHandler::createButton(string btnName, int posX, int posY, int h, int w, function<void()> inpf)
{
	InterfaceButton *newobj = new InterfaceButton(posX, posY, h, w, inpf);
	newobj->bindInterface(this);
	newobj->SetColour(rand() % 255, rand() % 255, rand() % 255, 150);
	menu->getCurrentMenu()->getButtonsMap().insert(make_pair( btnName, newobj));
}

void InterfaceHandler::update()
{
	menu->update();
	Cur->update();
}

InterfaceObject::InterfaceObject(int posX, int posY, int h, int w)
{
	SetColour(150, 150, 150, 255);
	pos.X = posX;
	pos.Y = posY;
	width = w;
	height = h;
}

position InterfaceObject::GetPos()
{
	return pos;
}

double InterfaceObject::GetWidth()
{
	return width;
}

double InterfaceObject::GetHeigth()
{
	return height;
}

void InterfaceObject::setPos(int PosX, int PosY)
{
	pos.X = PosX;
	pos.Y = PosY;
}

void InterfaceObject::setSize(int h, int w)
{
	width = w;
	height = h;
}



void InterfaceObject::bindInterface(InterfaceHandler * inp)
{
	sourceInterface = inp;
}

void InterfaceObject::SetColour(int Red, int Green, int Blue, int Alpha)
{
	colour[0] = limitInt(Red,225,0);
	colour[1] = limitInt(Green, 225, 0);
	colour[2] = limitInt(Blue, 225, 0);
	colour[3] = limitInt(Alpha, 225, 0);

}

void InterfaceObject::update()
{
	mouseCursor *curs = sourceInterface->Cur;
	if (this == curs->GetHoveredObj()) {
		isHovered = true;
		briteMult = 1.3f;
	}
	if (isHovered && this != curs->GetHoveredObj()) {
		isHovered = false;
		briteMult = 1.0f;
	}
}

InterfaceButton::InterfaceButton(int posX, int posY, int h, int w)
{
	pos.X = posX;
	pos.Y = posY;
	width = w;
	height = h;
	wasClicked = false;
	isClickable = true;
	buttonLable = new InterfaceLable(0, 0, 1, "");
}

InterfaceButton::InterfaceButton(int posX, int posY, int h, int w, function<void()> inpf)
{
	pos.X = posX;
	pos.Y = posY;
	width = w;
	height = h;
	SetAction(inpf);
	isClickable = true;
	wasClicked = false;
	buttonLable = new InterfaceLable(0, 0, 1, "");
}

InterfaceButton::InterfaceButton(int posX, int posY, int h, int w, string btnName, function<void()> inpf)
{
	pos.X = posX;
	pos.Y = posY;
	width = w;
	height = h;
	SetAction(inpf);
	isClickable = true;
	wasClicked = false;
	buttonLable = new InterfaceLable(posX + (w/4) - (5*btnName.size()), posY + (h/2)-10, 20, btnName);
}

void InterfaceButton::SetAction(function<void()> inpf)
{
	action = inpf;
}

void InterfaceButton::DoAction()
{
	briteMult = 1.2f;
	action();
}
void InterfaceButton::update()
{
	mouseCursor *curs = sourceInterface->Cur;
	if (this == curs->GetHoveredBtn()) {
		isHovered = true;
		briteMult = 1.4f;
		//cout << "On it!" << endl;
	}
	if (isHovered && this != curs->GetHoveredBtn()) {
		isHovered = false;
		briteMult = 1.0f;
	}
	if (isClickable && isHovered && curs->getClick() && !wasClicked) {
		//cout << "BAM!" << endl;
		DoAction();
		wasClicked = true;
	}
	if (!curs->getClick()) {
		wasClicked = false;
	}
}

void InterfaceButton::setPos(int PosX, int PosY)
{
	buttonLable->setPos(PosX, PosY);
	pos.X = PosX;
	pos.Y = PosY;
}

void InterfaceButton::setSize(int h, int w)
{
	width = w;
	height = h;
}


vector<float> InterfaceObject::draw(int SWidth, int SHight)
{
	vector<float> temp_buff;
	temp_buff.push_back((((-SWidth/2)+pos.X) / (SWidth/2)) + 0.5f); //ЛВ
	temp_buff.push_back((((SHight/2)-pos.Y)/ (SHight/2)) + 0.5f);
	temp_buff.push_back(1.0f);
	temp_buff.push_back((((-SWidth / 2) + pos.X + GetWidth()) / (SWidth / 2)) + 0.5f); // ПВ
	temp_buff.push_back((((SHight / 2) - pos.Y) / (SHight / 2)) + 0.5f);
	temp_buff.push_back(1.0f);
	temp_buff.push_back((((-SWidth / 2) + pos.X + GetWidth()) / (SWidth / 2)) + 0.5f); //ПН
	temp_buff.push_back((((SHight / 2) - pos.Y - GetHeigth()) / (SHight / 2)) + 0.5f);
	temp_buff.push_back(1.0f);
	temp_buff.push_back((((-SWidth / 2) + pos.X) / (SWidth / 2)) + 0.5f); //ЛВ
	temp_buff.push_back((((SHight / 2) - pos.Y) / (SHight / 2)) + 0.5f);
	temp_buff.push_back(1.0f);
	temp_buff.push_back((((-SWidth / 2) + pos.X + GetWidth()) / (SWidth / 2)) + 0.5f); // ПН
	temp_buff.push_back((((SHight / 2) - pos.Y - GetHeigth()) / (SHight / 2)) + 0.5f);
	temp_buff.push_back(1.0);
	temp_buff.push_back((((-SWidth / 2) + pos.X) / (SWidth / 2)) + 0.5f); //ЛН
	temp_buff.push_back((((SHight / 2) - pos.Y - GetHeigth()) / (SHight / 2)) + 0.5f);
	temp_buff.push_back(1.0f);
	return temp_buff;
}

vector<float> InterfaceObject::color()
{
	vector<float> temp_buff;
	for (int j = 0; j < 6; j++) {
		temp_buff.push_back(colour[0]); //лв 
		temp_buff.push_back(colour[1]);
		temp_buff.push_back(colour[2]);
		temp_buff.push_back(colour[3]*briteMult);
	}
	return temp_buff;
}

int limitInt(int inp, int Up, int Down)
{
	if (inp > Up) return Up;
	if (inp < Down) return Down;
	return inp;
}

InterfaceGlyph::InterfaceGlyph(int posX, int posY, int size, char sym)
{
	pos.X = posX;
	pos.Y = posY;
	calcSize(size);
	setSymb(sym);
}

void InterfaceGlyph::setSymb(char sym)
{
	symbol = sym;
	calcUVCords();
}

void InterfaceGlyph::calcSize(int size)
{
	width = size;
	height = size;
}
//Вычисление координат буквы на текстуре шрифта
void InterfaceGlyph::calcUVCords()
{
	float uv_x = (symbol % 16) / 16.0f;
	float uv_y = (symbol / 16) / 16.0f;
	float LU[2] = { uv_x                 , 1.0f - uv_y };
	float RU[2] = { uv_x + (1.0f / 16.0f), 1.0f - uv_y };
	float LD[2] = { uv_x				 , 1.0f - (uv_y + (1.0f / 16.0f)) };
	float RD[2] = { uv_x + (1.0f / 16.0f), 1.0f - (uv_y + (1.0f / 16.0f)) };
	UVCords[0] = LU[0];
	UVCords[1] = LU[1];
	UVCords[2] = RU[0];
	UVCords[3] = RU[1];
	UVCords[4] = RD[0];
	UVCords[5] = RD[1];
	UVCords[6] = LU[0];
	UVCords[7] = LU[1];
	UVCords[8] = RD[0];
	UVCords[9] = RD[1];
	UVCords[10] = LD[0];
	UVCords[11] = LD[1];
}

float* InterfaceGlyph::getUVCords()
{
	return UVCords;
}

InterfaceLable::InterfaceLable(int posX, int posY, int size, string inptext)
{
	pos.X = posX;
	pos.Y = posY;
	textsize = size;
	text = inptext;
	figureGlyphs();
}

void InterfaceLable::SetText(string inptext)
{
	text = inptext;
	figureGlyphs();
}

void InterfaceLable::figureGlyphs()
{
	glyphs.clear();
	for (int i = 0; i < text.size(); i++) {
		glyphs.push_back(InterfaceGlyph(pos.X + i*textsize, pos.Y, textsize, text[i]));
	}
}

void InterfaceLable::setSize(int size)
{
	textsize = size;
	figureGlyphs();
}

void InterfaceCheckbox::f_flip()
{
	check = !check;
	(check)?(checkLable->setSymb('+')):(checkLable->setSymb('-'));
}

InterfaceCheckbox::InterfaceCheckbox(int posX, int posY, int h, int w) : InterfaceButton(posX, posY, h, w)
{
	checkLable = new InterfaceGlyph(posX + (w / 4), posY + (h / 4), h / 2, '-');
	function<void()> f = std::bind(&InterfaceCheckbox::f_flip, this);
	SetAction(f);
}

void InterfaceCheckbox::DoAction() {
	f_flip();
}

bool InterfaceCheckbox::isChecked() {
	return check;
}

InterfaceMenu::InterfaceMenu(string name)
{
	menuName = name;
}

void InterfaceMenu::bindInter(InterfaceHandler * inpInter)
{
	sourceInterface = inpInter;
}

string InterfaceMenu::getName()
{
	return menuName;
}

void InterfaceMenu::addButton(string btnName, InterfaceButton *inpB)
{
	menuButtons.insert(std::make_pair(btnName,inpB));
	menuButtons[btnName]->bindInterface(sourceInterface);
}

void InterfaceMenu::addLable(string labName, InterfaceLable *inpL)
{
	menuLables.insert(std::make_pair(labName, inpL));
	menuLables[labName]->bindInterface(sourceInterface);
}

void InterfaceMenu::addCheckbox(string chName, InterfaceCheckbox * inpCh)
{
	menuCheckboxes.insert(std::make_pair(chName, inpCh));
	menuCheckboxes[chName]->bindInterface(sourceInterface);
}

void InterfaceMenu::addTextEdit(string TEName, InterfaceTextEdit * inpTE)
{
	menuTextEdits.insert(std::make_pair(TEName, inpTE));
	menuTextEdits[TEName]->bindInterface(sourceInterface);
}

void InterfaceMenu::bindActionToBtn(string btnName, function<void ()> inpF)
{
	menuButtons[btnName]->SetAction(inpF);
}

bool InterfaceMenu::getCheckboxState(string checkName)
{
	return menuCheckboxes[checkName]->isChecked();
}

string InterfaceMenu::getLableContent(string lableName)
{
	return menuLables[lableName]->getText();
}

int InterfaceMenu::getMenuSize()
{
	int sum = menuButtons.size() + menuLables.size() + menuCheckboxes.size();
	return sum;
}

void InterfaceMenu::update()
{
	map<string, InterfaceButton*>::iterator btn;
	map<string, InterfaceCheckbox*>::iterator ch;
	map<string, InterfaceTextEdit*>::iterator te;
	for (btn = menuButtons.begin(); btn != menuButtons.end(); btn++) {
		btn->second->update();
	}
	for (ch = menuCheckboxes.begin(); ch != menuCheckboxes.end(); ch++) {
		ch->second->update();
	}
	for (te = menuTextEdits.begin(); te != menuTextEdits.end(); te++) {
		te->second->update();
	}
}

MenuPackager::MenuPackager(InterfaceHandler* sorcI)
{
	sourceInterface = sorcI;
}

void MenuPackager::createMenu(string menuName)
{
	menus.insert(std::make_pair(menuName, new InterfaceMenu(menuName)));
	menus[menuName]->bindInter(sourceInterface);
}

InterfaceMenu * MenuPackager::getMenuByName(string menuName)
{
	return menus[menuName];
}

InterfaceMenu * MenuPackager::getCurrentMenu()
{
	return menus[activeMenuName];
}

int MenuPackager::getMenuCount()
{
	return menus.size();
}

void MenuPackager::update()
{
	getCurrentMenu()->update();
}

InterfaceTextEdit::InterfaceTextEdit(int posX, int posY, int h, int w) : InterfaceObject(posX,posY,h,w)
{
	curStr = new InterfaceLable(posX, posY, h, std::string());
	hadInp = false;
	lastInpT = glfwGetTime();
	maxChar = w / h;
}

string InterfaceTextEdit::getString()
{
	return curStr->getText();
}

InterfaceLable * InterfaceTextEdit::getLable()
{
	return curStr;
}

void InterfaceTextEdit::setText(string inpT)
{
	curStr->SetText(inpT);
}

void InterfaceTextEdit::resolveInp()
{
	if (isHovered) {
		double curT = glfwGetTime();
		if (curT - lastInpT > 0.2) {
			string tempStr = curStr->getText();
			if (GetAsyncKeyState(VK_BACK) && tempStr.size() > 0 && !hadInp) {
				tempStr.erase(tempStr.size() - 1, 1);
				curStr->SetText(tempStr);
				lastInpT = curT;
			}
			for (char testCh = ' '; testCh <= 'z'; testCh++) {
				if (GetKeyState(testCh) & 0x8000 && !hadInp && curStr->getText().size() < maxChar) {
					tempStr.append(1, testCh);
					curStr->SetText(tempStr);
					lastInpT = curT;
				}
			}
		}
	}
	
}

void InterfaceTextEdit::update()
{
	mouseCursor *curs = sourceInterface->Cur;
	if (this == curs->GetHoveredObj()) {
		isHovered = true;
	}
	if (isHovered && this != curs->GetHoveredObj()) {
		isHovered = false;
	}
	resolveInp();
}
