#include "Window.h"

struct cell{
	unsigned coordX;
	unsigned coordY;
	bool target;
	bool isFound;
	double targetChecker;
};

class Grid: protected virtual Window {
private:
	//розміри клітини
	unsigned cellWidth, cellHeight;

	//кількість клітин
	unsigned cellsInRow, cellsInColumn;

	//стартові координати та відступи
	unsigned x, y;
	unsigned bordX, bordY;

	int viewedTargs;
protected:
	cell** coords;
public:
	~Grid();

	void createCoords();

	void onlyRender();
	bool markTargets();

	void setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn);

	int getViewedTargets();
	unsigned getCellHeight();
	unsigned getCellsInRow();
	unsigned getCellsInColumn();
	unsigned getBord();

	void recalcTargets(int curIndexColumn, int curIndexRow, int curTargs);
	void isTarget(int curIndexColumn, int curIndexRow, int imp, int freq);
};


class Beam: protected virtual Window {
private:
	unsigned x, y;
	int radius;
public:
	void setValues(unsigned cellH, unsigned bord);

	void render(unsigned newX, unsigned newY);
	int move(cell** l, int curIndexColumn, int curIndexRow, unsigned cellH, int impCnt, int dImpCnt, bool first);

};