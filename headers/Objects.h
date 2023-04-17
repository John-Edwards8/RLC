#include "Render.h"

class Grid: protected virtual Render {
private:
	//cell dimensions
	int cellWidth, cellHeight;

	//number of cells
	unsigned int cellsInRow, cellsInColumn;

	//start coords
	int x, y;
	int bord_x, bord_y;

	comp** coords;
public:
	Grid();

	void createCoords();

	void onlyRender();

	void setBord(int scrW, int scrH);
	void setCellSize(int scrH);
	int getCellHeight();
	int getCellsInRow();
	int getCellsInColumn();
	int getBord();
	void setCellsCount(int cellsInRow, int cellsInColumn);
	comp** getGridCoords();

};


class Beam: protected virtual Render {
private:
	int x, y;
	int radius;
public:
	Beam();

	void setValues(int cellH, int bord);

	void render(int newX, int newY);
	void move(Render::comp** l, int curIndexColumn, int curIndexRow, int cellH);

};