#include <SDL2/SDL.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <ctime>
 
using namespace std;


class Window {
private:
	//Розміри екрану
	unsigned screenWidth, screenHeight;

	//Об'єкт вікна SDL
	SDL_Window* gWindow;

	void initWindow();

protected:
	//Об'єкт рендера SDL
	SDL_Renderer* rend;
public:
	Window();

	void setValues(unsigned width, unsigned height);

	bool init();
	void close();
	void _render();

	void reCreate();
	void _clear();
};
