#include "../headers/Manager.h"


Manager::Manager(){}

Manager::Manager(int screenWidth, int screenHeight, int cellsInRow, int cellsInColumn) : Window(screenWidth,screenHeight) {
	Grid::setCellsCount(cellsInRow, cellsInColumn);
	setStartValues(screenWidth, screenHeight);
}
Window Manager::getWindow() {
	Window::_getWindow();
}

void Manager::initGrid() {
	Grid::createCoords();
}

void Manager::clear() {
	Render::_clear();
	Grid::onlyRender();
}

void Manager::mark() {
	Render::_clear();
	Grid::onlyRender();
	Grid::markTargets();
	Render::_render();
}

void Manager::setStartValues(int screenWidth, int screenHeight) {
	Grid::setBord(screenWidth, screenHeight);
	Grid::setCellSize(screenHeight);

	Beam::setValues(Grid::getCellHeight(),Grid::getBord());
}

void Manager::moveBeam(int mvcnt) {
	int clsInRow = Grid::getCellsInRow();
	int clsInCol = Grid::getCellsInColumn();

	for (int i = 0; i < clsInCol; i++)	{
		for (int j = 0; j < clsInRow; j++)	{
			clear();
			Beam::move(Grid::getGridCoords(), i, j, Grid::getCellHeight());
			Render::_render();
			Render::_clear();
			if (!mvcnt) { SDL_Delay(25); }
			else { SDL_Delay(250); }
		}
	}
	if (mvcnt) {
		mark();
		log(Grid::getGridCoords(), clsInCol, clsInRow);
	}
}

void Manager::log(Render::comp** l, int clsInCol, int clsInRow){
	ofstream file("logs.txt", ios::app);

	time_t tt;
    struct tm* ti;
    time(&tt);
    ti = localtime(&tt);
  
	file << endl << asctime(ti) << endl;
	for(int i = 0; i < clsInCol; i++) {
		for (int j = 0; j < clsInRow; j++) {
			if((*(l + j) + i)->targetChecker > 0.55) { 
				file << "Row " << i+1 << "," << " and column " << j+1 << ", have a target." << endl;
			}
		}
	}
	file.close();
}
