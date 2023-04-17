#include "Objects.h"

class Manager: public Grid, public Beam {
public:
	Manager();

	Manager(int screenWidth, int screenHeight, int cellsInRow, int cellsInColumn);
	
	Window getWindow();
	void initGrid();

	void clear();

	void setStartValues(int screenWidth, int screenHeight);

	void moveBeam(int mvcnt);
};