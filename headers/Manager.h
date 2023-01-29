#include "Objects.h"

class Manager: public Grid, public Beam {
public:
	Manager(){}
	
	Window getWindow(){
		Window::getWindow();
	}

	void renderAll(){
		Render::_clear();
		Grid::render();
		Beam::render();
		Render::_render();
	}
};