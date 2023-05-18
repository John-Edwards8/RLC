#include "../headers/Objects.h"

Grid::~Grid() {
	for (int i = 0; i < this->cellsInRow; i++) {
		delete *(this->coords+i);
	}
	delete[] this->coords;
}

void Grid::createCoords(){
	srand(time(NULL));
	this->x = 0;
	this->y = this->bordY;
	this->coords = new cell*[this->cellsInRow];
	for (int i = 0; i < this->cellsInRow; i++) {
		*(this->coords+i) = new cell[this->cellsInColumn];
	}

	for (int i = 0; i < this->cellsInColumn; i++) {
		for (int j = 0; j < this->cellsInRow; j++) {
			(*(coords+j)+i)->coordX = this->x + this->bordX + (this->cellWidth*j);
			(*(coords+j)+i)->coordY = this->y + this->bordY + (this->cellHeight*i);
			(*(coords+j)+i)->target = ((rand()*100) % 8) > 0? true: false;
			(*(coords+j)+i)->isFound = false;
			(*(coords+j)+i)->targetChecker = 0.5;
		}
	}
}

void Grid::onlyRender() {
	SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
	for (int i = 0; i < this->cellsInColumn; i++)	{
		for (int j = 0; j < this->cellsInRow; j++)	{
			this->x = (*(coords+j)+i)->coordX;
			this->y = (*(coords+j)+i)->coordY;
			SDL_Rect outlineRect = { this->x, this->y, this->cellWidth, this->cellHeight  };
			SDL_RenderDrawRect( this->rend, &outlineRect );
		}
	}
}

bool Grid::markTargets() {
	bool isT = false;
	SDL_SetRenderDrawColor( this->rend, 0xFF, 0x00, 0x00, 0xFF );
	for (int i = 0; i < this->cellsInColumn; i++)	{
		for (int j = 0; j < this->cellsInRow; j++)	{
			if((*(coords+j)+i)->targetChecker >= 0.9) {
				isT = true;
				this->x = (*(coords+j)+i)->coordX;
				this->y = (*(coords+j)+i)->coordY;
				SDL_Rect outlineRect = { this->x+5, this->y+5, this->cellWidth-10, this->cellHeight-10  };
				SDL_RenderFillRect( this->rend, &outlineRect );
			}
		}
	}
	return isT;
}

void Grid::setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn){
	this->cellsInRow = cellsInRow;
	this->cellsInColumn = cellsInColumn;
	this->bordX = this->cellsInRow <= this->cellsInColumn? (scrW-scrH)/2 : (scrW-scrH)/6;
	this->cellWidth = this->cellHeight = (scrH/(this->cellsInColumn >= this->cellsInRow? this->cellsInColumn: this->cellsInRow))-this->bordY/(this->cellsInRow >= 30 || this->cellsInColumn >= 30? 20 : 8);
}

int Grid::getViewedTargets() { return this->viewedTargs; }
unsigned Grid::getCellHeight() { return this->cellHeight; }
unsigned Grid::getCellsInRow() { return this->cellsInRow; }
unsigned Grid::getCellsInColumn() {	return this->cellsInColumn; }
unsigned Grid::getBord() { return this->bordX; }


void Grid::recalcTargets(int curIndexColumn, int curIndexRow, int curTargs) {
	if(this->viewedTargs == curTargs && curTargs != 0) return;
	auto c = (*(this->coords + curIndexRow) + curIndexColumn);
	if(!c->target) return;
	this->viewedTargs++;
}

void Grid::isTarget(int curIndexColumn, int curIndexRow, int imp, int freq) {
	if((*(this->coords + curIndexRow) + curIndexColumn)->targetChecker >= 0.9 && (*(this->coords + curIndexRow) + curIndexColumn)->isFound == false) {
		cout << "У строці " << curIndexColumn+1 << "," << " та колонці " << curIndexRow+1 << ", знайдено ціль за " << (double)imp/freq << " секунд." << endl;
	}
}

//##########################################################################################

void Beam::setValues(unsigned cellH, unsigned bord) {
	this->x = cellH/2 + bord;
	this->y = cellH/2;
	this->radius = cellH/4;
}

void Beam::render(unsigned newX, unsigned newY) {
	SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0xFF, 0xFF);
    for (int w = 0; w < this->radius * 2; w++) {
        for (int h = 0; h < this->radius * 2; h++) {
            int dx = this->radius - w;
            int dy = this->radius - h;
            if ((dx*dx + dy*dy) <= (this->radius * this->radius)) {
                SDL_RenderDrawPoint(this->rend, newX + dx, newY + dy);
            }
        }
    }
}

int Beam::move(cell** l, int curIndexColumn, int curIndexRow, unsigned cellH, int impCnt, int dImpCnt, bool first) {
	auto c = (*(l + curIndexRow) + curIndexColumn);

	render(c->coordX + cellH/2, c->coordY + cellH/2);
	if(!first) { cout << "У клітинку (" << curIndexColumn+1 << "," << curIndexRow+1 << ") направлено " << (!c->target? impCnt : dImpCnt) << " імпульсів." << endl; }
	else { cout << "У клітинку (" << curIndexColumn+1 << "," << curIndexRow+1 << ") направлено " << impCnt << " імпульсів." << endl; }

	if (!c->target) { return impCnt; }

	c->targetChecker += dImpCnt*0.001; //100 імпульсів = +0.1 ймовірності

	cout << "Отримано позитивну відповідь." << endl;
	if (c->target && !first) { return dImpCnt; }
	else return impCnt;
}
