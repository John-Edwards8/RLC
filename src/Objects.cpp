#include "../headers/Objects.h"


Grid::Grid(){}

void Grid::createCoords(){
	srand(time(NULL));
	this->x = 0;
	this->y = this->bord_y;
	coords = new comp*[this->cellsInRow];
	for (int i = 0; i < this->cellsInRow; i++) {
		*(coords+i) = new comp[this->cellsInColumn];
	}

	for (int i = 0; i < this->cellsInColumn; i++) {
		for (int j = 0; j < this->cellsInRow; j++) {
			(*(coords+j)+i)->coordX = this->x + this->bord_x + (cellWidth*j);
			(*(coords+j)+i)->coordY = this->y + this->bord_y + (cellHeight*i);
			(*(coords+j)+i)->target = rand() % 10000 < 2? true:false;
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

void Grid::setBord(int scrW, int scrH) {
	this->bord_x = this->cellsInRow <= this->cellsInColumn? (scrW-scrH)/2 : (scrW-scrH)/6;
}

void Grid::setCellSize(int scrH) {
	this->cellWidth = this->cellHeight = (scrH/(this->cellsInColumn >= this->cellsInRow? this->cellsInColumn: this->cellsInRow))-this->bord_y/(this->cellsInRow >= 30 || this->cellsInColumn >= 30? 20 : 8);
}

int Grid::getCellHeight() {
	return this->cellHeight;
}

int Grid::getCellsInRow() {
	return this->cellsInRow;
}

int Grid::getCellsInColumn() {
	return this->cellsInColumn;
}

int Grid::getBord() {
	return this->bord_x;
}
void Grid::setCellsCount(int cellsInRow, int cellsInColumn) {
	this->cellsInRow = cellsInRow;
	this->cellsInColumn = cellsInColumn;
}
Render::comp** Grid::getGridCoords() {
	return this->coords;
}

Beam::Beam(){}

void Beam::setValues(int cellH, int bord) {
	this->x = cellH/2 + bord;
	this->y = cellH/2;
	this->radius = cellH/4;
}

void Beam::render(int newX, int newY) {
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

void Beam::move(Render::comp** l, int curIndexColumn, int curIndexRow, int cellH) {
	auto c = (*(l + curIndexRow) + curIndexColumn);

	if(c->targetChecker == 1){ return; }
	
	c->targetChecker += c->target? 10: 1;

	render(c->coordX + cellH/2, c->coordY + cellH/2);
}
