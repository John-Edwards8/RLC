#include "../headers/Manager.h"

Manager::Manager(){}

bool Manager::init() { return Window::init(); }
void Manager::close() { Window::close(); }

void Manager::_render() {
	if(done) { return; }
	Window::_render();
}

void Manager::initGrid() { Grid::createCoords(); }

void Manager::renderGrid() {
	if(done) { return; }
	Window::_clear();
	Grid::onlyRender();
}

void Manager::mark() {
	renderGrid();
	Grid::markTargets();
	Window::_render();
}

void Manager::moveBeam(int impCnt) {
	if(done) { return; }
	int clsInRow = Grid::getCellsInRow();
	int clsInCol = Grid::getCellsInColumn();

	for (int i = 0; i < clsInCol; i++)	{
		for (int j = 0; j < clsInRow; j++)	{
			renderGrid();
			Beam::move(this->coords, i, j, Grid::getCellHeight(), impCnt/(clsInRow*clsInCol));
			impCnt -= impCnt/(clsInRow*clsInCol);
			Window::_render();
			Window::_clear();
			SDL_Delay(250);
		}
	}
	mark();
	done = log(this->coords, clsInCol, clsInRow);
}

bool Manager::log(comp** l, int clsInCol, int clsInRow){
	bool check = false;
	ofstream file("logs.txt", ios::app);

	time_t tt;
    struct tm* ti;
    time(&tt);
    ti = localtime(&tt);
  
	file << endl << asctime(ti) << endl;
	for(int i = 0; i < clsInCol; i++) {
		for (int j = 0; j < clsInRow; j++) {
			if((*(l + j) + i)->targetChecker >= 0.9) { 
				file << "Row " << i+1 << "," << " and column " << j+1 << ", have a target." << endl;
				check = true;
			}
		}
	}
	file.close();
	return check;
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
	Grid::setBord(abs(w), abs(h));
	Grid::setCellSize(abs(h));
	Beam::setValues(Grid::getCellHeight(),Grid::getBord());
}