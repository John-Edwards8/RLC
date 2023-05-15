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

void Manager::moveBeam(int impCnt, int freq) {
	if(done) { return; }
	int clsInRow = Grid::getCellsInRow();
	int clsInCol = Grid::getCellsInColumn();

	targets = Grid::getViewedTargets();
	if( targets != 0 ) { doubleImpForCell = impForCell*2; first = false; }
	else if( targets == 0 && first == false ) { done = true; return; }
	impForCell = (impCnt-doubleImpForCell*targets)/(clsInRow*clsInCol-targets);

	for (int i = 0; i < clsInCol; i++)	{
		for (int j = 0; j < clsInRow; j++)	{
			renderGrid();
			allImp += Beam::move(this->coords, i, j, Grid::getCellHeight(), impForCell, doubleImpForCell, first);
			Grid::isTarget(this->coords, i, j, allImp, freq);
			Grid::recalcTargets(this->coords, i, j, targets);
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
				file << "Строка " << i+1 << "," << " та колонка " << j+1 << ", мають ціль." << endl;
				check = true;
			}
		}
	}
	file.close();
	return check;
}

void Manager::setValues() {
	int w, h, r, c;
	cout << "Введіть ширину вікна:" << endl;
	if (!(cin >> w)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Неправильно введена ширина!");
	}
	cout << "Введіть висоту вікна:" << endl;
	if (!(cin >> h)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Неправильно введена висота!");
	}
	cout << "Введіть значення клітин сітки (скільки в строці та в колонці):" << endl;
	if (!(cin >> r >> c)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Неправильно введені значення!");
	}

	Window::setSize(abs(w), abs(h));
	Window::reCreate();
	Grid::setCellsCount(abs(r), abs(c));
	Grid::setBord(abs(w), abs(h));
	Grid::setCellSize(abs(h));
	Beam::setValues(Grid::getCellHeight(),Grid::getBord());
}