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

	void start() {
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

	void moveBeam() {
		for(int i = 1; i <= (Grid::getCellsInRow()*Grid::getCellsInColumn()); i++) {
			clear();
			Beam::move(Grid::getGridCoords(), i, Grid::getCellHeight());
			Render::_buffered();
			Render::_render();
			Render::_unbuffered();
			Render::_clear();
			SDL_Delay(50);
		}
	}
};