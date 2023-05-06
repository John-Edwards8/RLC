#include "Objects.h"

class Manager: public Grid, public Beam {
public:
	Manager();

	Manager(unsigned screenWidth, unsigned screenHeight, unsigned cellsInRow, unsigned cellsInColumn);

	bool init();
	void close();

	void _render();

	void initGrid();

	void clear();
	void mark();

	void setStartValues(unsigned screenWidth, unsigned screenHeight);

	void moveBeam(int mvcnt, int impCnt);

	void log(comp** l, int clsInCol, int clsInRow);

	void setValues();
};