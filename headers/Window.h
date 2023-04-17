#include "Prototypes.h"

class Window {
private:
	//Screen dimension
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;

	//The window we'll be rendering to
	SDL_Window* gWindow;

	void initWindow();
public:
	Window();
	Window(int screenWidth, int screenHeight);

	int getWindowWidth();
	int getWindowHeight();
	SDL_Window * getWindow();
	Window _getWindow();

	void setSize(int width, int height);


	bool init();
	void close();
};


