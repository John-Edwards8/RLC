#include "Render.h"

class Grid: protected virtual Render {
private:
	//cell dimensions
	double cellWidth;
	double cellHeight;

	//number of cells
	unsigned int cellsInRow;    //x && w
	unsigned int cellsInColumn; //y && h

	//start coords
	double x, y;
	double bord_x;
	//double bord_y;
public:
	Grid(){}
	// Grid(int screenWidth, int screenHeight) : Grid() {}
	void render() {
		x = y = 0;
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
		for (int i = 0; i < cellsInColumn; i++)	{
			for (int j = 0; j < cellsInRow; j++)	{
				SDL_Rect outlineRect = { this->x + this->bord_x + (cellWidth*j), this->y, this->cellWidth, this->cellHeight  };
				SDL_RenderDrawRect( this->rend, &outlineRect );
			}
			y+=cellHeight;
		}
	}
	void setBord(int scrW, int scrH) {
		this->bord_x = this->cellsInRow <= this->cellsInColumn? (scrW-scrH)/2 : (scrW-scrH)/6;
		//this->bord_y = this->cellsInColumn <= this->cellsInRow? (scrW-scrH)%2 : (scrW-scrH)%6;
	}

	void setCellSize(int scrH) {
		this->cellWidth = this->cellHeight = scrH/cellsInColumn;
	}

	double getCellHeight() {
		return this->cellHeight;
	}

	double getBord() {
		return this->bord_x;
	}
	void setCellsCount(int cellsInRow, int cellsInColumn){
		this->cellsInRow = cellsInRow;
		this->cellsInColumn = cellsInColumn;
	}
};


class Beam: protected virtual Render {
private:
	double x, y;
	double radius;
public:
	Beam(){}
	// Beam(int screenWidth, int screenHeight) : Beam() {}
	void setValues(double cellH, double bord) {
		this->x = cellH/2 + bord;
		this->y = cellH/2;
		this->radius = cellH/4;
	}

	void render() {
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0xFF, 0xFF);
	    for (int w = 0; w < this->radius * 2; w++)
	    {
	        for (int h = 0; h < this->radius * 2; h++)
	        {
	            double dx = this->radius - w;
	            double dy = this->radius - h;
	            if ((dx*dx + dy*dy) <= (this->radius * this->radius))
	            {
	                SDL_RenderDrawPoint(this->rend, this->x + dx, this->y + dy);
	            }
	        }
	    }
	}
/*
	void move() {
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0xFF, 0xFF);
	    for (int w = 0; w < radius * 2; w++)
	    {
	        for (int h = 0; h < radius * 2; h++)
	        {
	            int dx = radius - w;
	            int dy = radius - h;
	            if ((dx*dx + dy*dy) <= (radius * radius))
	            {
	                SDL_RenderDrawPoint(this->rend, x + dx, y + dy);
	            }
	        }
	    }
	}
*/	

};