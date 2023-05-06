#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
 
using namespace std;


class Window {
private:
	//Screen dimension
	unsigned SCREEN_WIDTH, SCREEN_HEIGHT;

	//The window we'll be rendering to
	SDL_Window* gWindow;

	void initWindow();

protected:
	SDL_Renderer* rend;
public:
	Window();
	Window(unsigned screenWidth, unsigned screenHeight);

	unsigned getWindowWidth();
	unsigned getWindowHeight();

	void setSize(unsigned width, unsigned height);

	virtual bool init();
	virtual void close();

	void reCreate();
	virtual void _render();
	void _clear();
};
