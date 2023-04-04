#include "Objects.h"

class Manager: public Grid, public Beam {
public:
	Manager(){}

	Manager(int screenWidth, int screenHeight, int cellsInRow, int cellsInColumn) : Window(screenWidth,screenHeight) {
		Grid::setCellsCount(cellsInRow, cellsInColumn);
		setStartValues(screenWidth, screenHeight);
	}
	Window getWindow() {
		Window::_getWindow();
	}

	void initGrid() {
		Grid::createCoords();
	}

	void clear() {
		Render::_clear();
		Grid::onlyRender();
	}

	void setStartValues(int screenWidth, int screenHeight) {
		Grid::setBord(screenWidth, screenHeight);
		Grid::setCellSize(screenHeight);

		Beam::setValues(Grid::getCellHeight(),Grid::getBord());
	}

	void moveBeam(int mvcnt) {
		int clsInRow = Grid::getCellsInRow();
		int clsInCol = Grid::getCellsInColumn();
 
		for (int i = 0; i < clsInCol; i++)	{
			for (int j = 0; j < clsInRow; j++)	{
				clear();
				Beam::move(Grid::getGridCoords(), i, j, Grid::getCellHeight());
				Render::_render();
				Render::_clear();
				if (!mvcnt) { SDL_Delay(25); }
				else { SDL_Delay(250); }
			}
		}
	}

	/*void moveBeam() {
		for(int i = 1; i <= (Grid::getCellsInRow()*Grid::getCellsInColumn()); i++) {
			clear();
			Beam::move(Grid::getGridCoords(), i, Grid::getCellHeight());
			Render::_render();
			Render::_clear();
			switch ( this->stage ) {
			case 1:
				SDL_Delay(300);
				break;
			default:
				SDL_Delay(50);
			}
		}
		this->stage++;
	}*/
};