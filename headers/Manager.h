#include "Objects.h"

class Manager: public Grid, public Beam {
private:
	bool done = false;
public:
	Manager();

	Manager(unsigned screenWidth, unsigned screenHeight, unsigned cellsInRow, unsigned cellsInColumn);

	virtual bool init();
	virtual void close();
	virtual void _render();

	void initGrid();
	void renderGrid();
	void mark();

	void moveBeam(int impCnt);

	bool log(comp** l, int clsInCol, int clsInRow);

	void setValues();
};