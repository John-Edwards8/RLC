#include "../headers/Window.h"

void Window::initWindow() {
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
		exit(1);
	}
	else {
		//Create window
		SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gWindow, &rend);
		//SDL_HideWindow( this->gWindow );
	}
}

Window::Window(){ initWindow(); }
Window::Window(unsigned screenWidth, unsigned screenHeight) { initWindow(); setSize(screenWidth, screenHeight); reCreate(); }

unsigned Window::getWindowWidth() { return this->SCREEN_WIDTH; }
unsigned Window::getWindowHeight() {	return this->SCREEN_HEIGHT; }

void Window::setSize(unsigned width, unsigned height) {
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	SDL_DestroyWindow( this->gWindow );
	this->gWindow = SDL_CreateWindow( "RLC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
}


bool Window::init(){
	bool success = SDL_Init( SDL_INIT_VIDEO ) < 0? false : true;

	if( !success ) { cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl; }

	return success;
}

void Window::close() { 
	//Destroy window	
	SDL_DestroyWindow( this->gWindow );
	this->gWindow = NULL;
	SDL_DestroyRenderer( this->rend );
	this->rend = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void Window::reCreate() {
	SDL_DestroyRenderer( this->rend );
	this->rend = SDL_CreateRenderer( this->gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}

void Window::_render() {
	SDL_RenderPresent( this->rend );
}

void Window::_clear() {
	SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( this->rend );
}

