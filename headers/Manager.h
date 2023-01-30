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

	void start(){
		Render::_clear();
		Grid::render();
		Beam::render();
		Render::_render();
	}

	void setStartValues(int screenWidth, int screenHeight){
		Grid::setBord(screenWidth, screenHeight);
		Grid::setCellSize(screenHeight);

		Beam::setValues(Grid::getCellHeight(),Grid::getBord());

	}
};