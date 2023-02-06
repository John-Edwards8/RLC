#include "Render.h"

class Grid: protected virtual Render {
private:
	//cell dimensions
	int cellWidth, cellHeight;

	//number of cells
	unsigned int cellsInRow, cellsInColumn;

	//start coords
	int x, y;
	int bord_x, bord_y;

	GridList coords;
public:
	Grid(){}

	void createCoords(){
		this->x = 0;
		this->y = this->bord_y;
		constr_list(coords);
		for (int i = 0; i < this->cellsInColumn; i++)	{
			for (int j = 0; j < this->cellsInRow; j++)	{
				comp_in(coords, (this->x + this->bord_x + (cellWidth*j)), (this->y+this->bord_y+(cellHeight*i)));
			}
		}

	}
/*
	void startRender() {
		this->x = 0;
		this->y = this->bord_y;
		constr_list(coords);
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
		for (int i = 0; i < this->cellsInColumn; i++)	{
			for (int j = 0; j < this->cellsInRow; j++)	{
				SDL_Rect outlineRect = { this->x + this->bord_x + (cellWidth*j), this->y+this->bord_y+(cellHeight*i), this->cellWidth, this->cellHeight  };
				SDL_RenderDrawRect( this->rend, &outlineRect );
				comp_in(coords, (this->x + this->bord_x + (cellWidth*j)), (this->y+this->bord_y+(cellHeight*i)));
			}
		}
	}
*/
	void onlyRender() {
		comp* c = coords.head;
		this->x = c->coordX;
		this->y = c->coordY;
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
		for (int i = 0; i < this->cellsInColumn; i++)	{
			for (int j = 0; j < this->cellsInRow; j++)	{
				SDL_Rect outlineRect = { this->x, this->y, this->cellWidth, this->cellHeight  };
				SDL_RenderDrawRect( this->rend, &outlineRect );
				if(c->next != NULL) { c = c->next; }
				else{ break; }
				this->x = c->coordX;
			}
			this->y = c->coordY;
		}
	}

	void setBord(int scrW, int scrH) {
		this->bord_x = this->cellsInRow <= this->cellsInColumn? (scrW-scrH)/2 : (scrW-scrH)/6;
	}

	void setCellSize(int scrH) {
		this->cellWidth = this->cellHeight = (scrH/(this->cellsInColumn >= this->cellsInRow? this->cellsInColumn: this->cellsInRow))-this->bord_y/(this->cellsInRow >= 30 || this->cellsInColumn >= 30? 20 : 8);
	}

	int getCellHeight() {
		return this->cellHeight;
	}

	int getCellsInRow() {
		return this->cellsInRow;
	}

	int getCellsInColumn() {
		return this->cellsInColumn;
	}

	int getBord() {
		return this->bord_x;
	}
	void setCellsCount(int cellsInRow, int cellsInColumn) {
		this->cellsInRow = cellsInRow;
		this->cellsInColumn = cellsInColumn;
	}
	GridList getGridCoords() {
		return this->coords;
	}

};


class Beam: protected virtual Render {
private:
	int x, y;
	int radius;
public:
	Beam(){}

	void setValues(int cellH, int bord) {
		this->x = cellH/2 + bord;
		this->y = cellH/2;
		this->radius = cellH/4;
	}

	/*void startRender() {
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0xFF, 0xFF);
	    for (int w = 0; w < this->radius * 2; w++) {
	        for (int h = 0; h < this->radius * 2; h++) {
	            int dx = this->radius - w;
	            int dy = this->radius - h;
	            if ((dx*dx + dy*dy) <= (this->radius * this->radius)) {
	                SDL_RenderDrawPoint(this->rend, this->x + dx, this->y + dy);
	            }
	        }
	    }
	}*/

	void render(int newX, int newY) {
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
	
	void move(Render::GridList l, int curIndex, int cellH) {
		comp* c = l.head;
		if(curIndex > (l.tail)->index){ return; }
		while(c->index != curIndex) { c = c->next; }
		render(c->coordX + cellH/2, c->coordY + cellH/2);
	}

};