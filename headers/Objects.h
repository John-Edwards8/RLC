#include "Window.h"

struct comp{
	unsigned coordX;
	unsigned coordY;
	bool target;
	bool isFound;
	double targetChecker;
};

class Grid: protected virtual Window {
private:
	//cell dimensions
	unsigned cellWidth, cellHeight;

	//number of cells
	unsigned cellsInRow, cellsInColumn;

	//start coords
	unsigned x, y;
	unsigned bord_x, bord_y;

	int viewedTargs;
protected:
	comp** coords;
public:
	Grid();

	void createCoords();

	void onlyRender();
	bool markTargets();

	void setBord(unsigned scrW, unsigned scrH);
	void setCellSize(unsigned scrH);
	unsigned getCellHeight();
	unsigned getCellsInRow();
	unsigned getCellsInColumn();
	unsigned getBord();
	void setCellsCount(unsigned cellsInRow, unsigned cellsInColumn);

	void recalcTargets(comp** l, int curIndexColumn, int curIndexRow, int curTargs);
	int getViewedTargets();
	void isTarget(comp** l, int curIndexColumn, int curIndexRow, int imp, int freq);
};


class Beam: protected virtual Window {
private:
	unsigned x, y;
	int radius;
public:
	Beam();

	void setValues(unsigned cellH, unsigned bord);

	void render(unsigned newX, unsigned newY);
	int move(comp** l, int curIndexColumn, int curIndexRow, unsigned cellH, int impCnt, int dImpCnt, bool first);

};