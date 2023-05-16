#include "Objects.h"

class Manager: public Grid, public Beam {
private:
	bool done = false;
	bool logging = false;
	bool first = true;
	int impForCell = 0;
	int doubleImpForCell = 0;
	int targets = 0;
	int allImp = 0;
public:
	Manager();

	Manager(unsigned screenWidth, unsigned screenHeight, unsigned cellsInRow, unsigned cellsInColumn);

	virtual bool init();
	virtual void close();
	virtual void _render();

	void initGrid();
	void renderGrid();
	void mark();

	void moveBeam(int impCnt, int freq);

	void log(comp** l, int cI, int cJ, int imp, int freq, int impCnt, int dImpCnt);

	void setValues();
};