#include "Render.h"

class Grid : virtual private Render{
private:
	//cell dimensions
	int cellWidth = 42;
	int cellHeight = 21;

	//number of cells
	unsigned int cellsInRow = 30;    //x && w
	unsigned int cellsInColumn = 30; //y && h

	//start coords
	int x = 0, y = 0;

	//standard border
	int bord_x = 10;
	int bord_y = 45;

public:
	Grid(){}
	void render() {
		SDL_SetRenderDrawColor( this->rend, 0x00, 0x00, 0x00, 0xFF );
		for (int i = 0; i < cellsInColumn; i++)	{
			for (int j = 0; j < cellsInRow; j++)	{
				SDL_Rect outlineRect = { x + bord_x + (cellWidth*j), y + bord_y, this->cellWidth, this->cellHeight  };
				SDL_RenderDrawRect( this->rend, &outlineRect );
			}
			y+=cellHeight;
		}
		_render();
	}
	Window getWindow(){
		Window::getWindow();
	}
};