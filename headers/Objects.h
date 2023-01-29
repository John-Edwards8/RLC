#include "Render.h"

class Grid: protected virtual Render {
private:
	//cell dimensions
	int cellWidth = 42;
	int cellHeight = 20;

	//number of cells
	unsigned int cellsInRow = 30;    //x && w
	unsigned int cellsInColumn = 30; //y && h

	//start coords
	int x, y;

	//standard border
	int bord_x = 10;
	int bord_y = 50;
public:
	Grid(){}
	void render() {
		x = y = 0;
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
		for (int i = 0; i < cellsInColumn; i++)	{
			for (int j = 0; j < cellsInRow; j++)	{
				SDL_Rect outlineRect = { this->x + this->bord_x + (cellWidth*j), this->y + this->bord_y, this->cellWidth, this->cellHeight  };
				SDL_RenderDrawRect( this->rend, &outlineRect );
			}
			y+=cellHeight;
		}
	}

};


class Beam: protected virtual Render {
private:
	//start coords
	int x = 31, y = 60;

	int radius = 7;
public:
	Beam(){}
	void render() {
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
};