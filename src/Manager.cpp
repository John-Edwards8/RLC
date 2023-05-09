#include "../headers/Manager.h"

Manager::Manager(){}

bool Manager::init() { return Window::init(); }
void Manager::close() { Window::close(); }

void Manager::_render() { Window::_render(); }

Manager::Manager(unsigned screenWidth, unsigned screenHeight, unsigned cellsInRow, unsigned cellsInColumn) : Window(screenWidth,screenHeight) {
	Grid::setCellsCount(cellsInRow, cellsInColumn);
	setStartValues(screenWidth, screenHeight);
}

void Manager::initGrid() {
	Grid::createCoords();
}

void Manager::clear() {
	Window::_clear();
	Grid::onlyRender();
}

void Manager::mark() {
	Window::_clear();
	Grid::onlyRender();
	Grid::markTargets();
	Window::_render();
}

void Manager::setStartValues(unsigned screenWidth, unsigned screenHeight) {
	Grid::setBord(screenWidth, screenHeight);
	Grid::setCellSize(screenHeight);

	Beam::setValues(Grid::getCellHeight(),Grid::getBord());
}

void Manager::moveBeam(int mvcnt, int impCnt) {
	int clsInRow = Grid::getCellsInRow();
	int clsInCol = Grid::getCellsInColumn();

	for (int i = 0; i < clsInCol; i++)	{
		for (int j = 0; j < clsInRow; j++)	{
			clear();
			Beam::move(this->coords, i, j, Grid::getCellHeight(), impCnt/(clsInRow*clsInCol));
			impCnt -= impCnt/(clsInRow*clsInCol);
			Window::_render();
			Window::_clear();
			if (!mvcnt) { SDL_Delay(25); }
			else { SDL_Delay(250); }
		}
	}
	if (mvcnt) {
		mark();
		log(this->coords, clsInCol, clsInRow);
	}
}

void Manager::log(comp** l, int clsInCol, int clsInRow){
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

void Manager::setValues() {
	int w, h, r, c;
	cout << "Enter window width:" << endl;
	if (!(cin >> w)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Incorrect width!");
	}
	cout << "Enter window height:" << endl;
	if (!(cin >> h)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Incorrect height!");
	}
	cout << "Enter value of grid's cells (in row, in column):" << endl;
	if (!(cin >> r >> c)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Incorrect values!");
	}

	Window::setSize(abs(w), abs(h));
	Window::reCreate();
	Grid::setCellsCount(abs(r), abs(c));
	setStartValues(abs(w), abs(h));
}