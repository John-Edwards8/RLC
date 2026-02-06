#include "../headers/Manager.h"

bool Manager::init() {
	Window::init();
	return true;
}
void Manager::close() { Window::close(); }
void Manager::_render() { if(!this->done) Window::_render(); }

void Manager::initGrid() { Grid::createCoords(); }
void Manager::renderGrid() {
	if(!this->done) {
		Window::_clear();
		Grid::onlyRender();
	}
}

void Manager::mark() {
	renderGrid();
	this->done = Grid::markTargets();
	Window::_render();
}

void Manager::moveBeam(int impCnt, int freq) {
	if(this->done) { return; }
	int clsInRow = Grid::getCellsInRow();
	int clsInCol = Grid::getCellsInColumn();

	this->targets = Grid::getViewedTargets();
	if (this->targets != 0) {
		if (this->targets * this->impForCell * 2 <= impCnt) {
			this->doubleImpForCell = this->impForCell * 2;
		}
		if (this->doubleImpForCell) {
			this->doubleImpForCell = this->doubleImpForCell;
		}
		else {
			this->doubleImpForCell = this->impForCell;
		}
		this->first = false;
	} else if( this->targets == 0 && this->first == false ) {
		this->done = true;
		return;
	}
	
	this->impForCell = (impCnt-this->doubleImpForCell*this->targets)/(clsInRow*clsInCol-this->targets);

	for (int i = 0; i < clsInCol; i++)	{
		for (int j = 0; j < clsInRow; j++)	{
			renderGrid();
			this->allImp += Beam::move(this->coords, i, j, Grid::getCellHeight(), this->impForCell, this->doubleImpForCell, this->first);
			Grid::isTarget(i, j, this->allImp, freq);
			Grid::recalcTargets(i, j, this->targets);
			log(i, j, this->allImp, freq, this->impForCell, this->doubleImpForCell);
			Window::_render();
			Window::_clear();
			SDL_Delay(250);
		}
	}
	mark();
	if( this->targets == 0 ) { this->first = false; }
}

void Manager::log(int cI, int cJ, int imp, int freq, int impCnt, int dImpCnt){
	auto c = (*(this->coords + cJ) + cI);
	ofstream file("logs.txt", ios::app);
	if(this->first && !this->logging) {
		time_t tt;
	    struct tm* ti;
	    time(&tt);
	    ti = localtime(&tt);
	  
		file << endl << asctime(ti) << endl;
		this->logging = true;
	}
	if(!this->first) { file << "У клітинку (" << cI+1 << "," << cJ+1 << ") направлено " << (!c->target? impCnt : dImpCnt) << " імпульсів." << endl; }
	else { file << "У клітинку (" << cI+1 << "," << cJ+1 << ") направлено " << impCnt << " імпульсів." << endl; }

	if (c->target) { file << "Отримано позитивну відповідь." << endl; }

	if(c->targetChecker >= 0.9 && c->isFound == false) {
		file << "У строці " << cI+1 << "," << " та колонці " << cJ+1 << ", знайдено ціль за " << (double)imp/freq << " секунд." << endl;
	}
	file.close();
}

void Manager::setValues(int& fr, int& impulse) {
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
	cout << "Введіть загальну кількість імпульсів:" << endl;
	if (!(cin >> impulse)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Неправильно введена кількість!");
	}
	cout << "Введіть частоту імпульсів:" << endl;
	if (!(cin >> fr)) {
		cin.clear();
		while (cin.get() != '\n') continue;
		throw invalid_argument("Неправильно введена частота!");
	}

	Window::setValues(abs(w), abs(h)); Window::reCreate();
	Grid::setValues(abs(w), abs(h), abs(r), abs(c));
	Beam::setValues(Grid::getCellHeight(),Grid::getBord());
}