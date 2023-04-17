#include "Window.h"

class Render: protected virtual Window {
protected:
	SDL_Renderer* rend;

	struct comp{
		int coordX;
		int coordY;
		unsigned int index;
		unsigned int targetChecker;
		bool target;
	};

public:
	Render();

	void _render();
	void _clear();
};