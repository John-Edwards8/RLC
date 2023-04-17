#include "../headers/Window.h"

void Window::initWindow() {
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
		exit(1);
	}
	else {
		//Create window
		gWindow = SDL_CreateWindow( "RLC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	}
}

Window::Window(){ initWindow(); }
Window::Window(int screenWidth, int screenHeight) {setSize(screenWidth, screenHeight); initWindow();}

int Window::getWindowWidth() { return this->SCREEN_WIDTH; }
int Window::getWindowHeight() {	return this->SCREEN_HEIGHT; }
SDL_Window * Window::getWindow() { return this->gWindow; }
Window Window::_getWindow() { return *this; }

void Window::setSize(int width, int height) {
	SCREEN_WIDTH = width > 0? width : SCREEN_WIDTH;
	SCREEN_HEIGHT = height > 0? height : SCREEN_HEIGHT;
}


bool Window::init(){
	bool success = SDL_Init( SDL_INIT_VIDEO ) < 0? false : true;

	if( !success ) { cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl; }

	return success;
}

void Window::close() { 
	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}



