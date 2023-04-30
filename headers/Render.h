#include "Window.h"

class Render: protected virtual Window {
protected:
	SDL_Renderer* rend;

	struct comp{
		int coordX;
		int coordY;
		bool target;
		double targetChecker;
	};

public:
	Render();

	void _render();
	void _clear();
};