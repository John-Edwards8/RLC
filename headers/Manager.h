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
	bool init();
	void close();
	void _render();

	void initGrid();
	void renderGrid();

	void mark();

	void moveBeam(int impCnt, int freq);

	void log(int cI, int cJ, int imp, int freq, int impCnt, int dImpCnt);

	void setValues(int& fr, int& impulse);
};