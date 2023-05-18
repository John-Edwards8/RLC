#include "../headers/Window.h"

void Window::initWindow() {
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
		cout << "SDL не зміг ініціалізуватися! Помилка SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	else {
		//Create window
		SDL_CreateWindowAndRenderer(screenWidth, screenHeight, 0, &gWindow, &rend);
		SDL_HideWindow( this->gWindow );
	}
}

Window::Window(){ initWindow(); }

void Window::setValues(unsigned width, unsigned height) {
	this->screenWidth = width;
	this->screenHeight = height;
	SDL_DestroyWindow( this->gWindow );
	this->gWindow = SDL_CreateWindow( "RLC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
}


bool Window::init(){
	bool success = SDL_Init( SDL_INIT_VIDEO ) < 0? false : true;

	if( !success ) { cout << "SDL не зміг ініціалізуватися! Помилка SDL: " << SDL_GetError() << endl; }

	return success;
}

void Window::close() { 
	//Знищити об'єкт вікна	
	SDL_DestroyWindow( this->gWindow );
	this->gWindow = NULL;
	SDL_DestroyRenderer( this->rend );
	this->rend = NULL;

	//Вийти з підсистем SDL
	SDL_Quit();
}

void Window::reCreate() {
	SDL_DestroyRenderer( this->rend );
	this->rend = SDL_CreateRenderer( this->gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}

void Window::_render() { SDL_RenderPresent( this->rend ); }

//зафарбувати білим кольором
void Window::_clear() {
	SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( this->rend );
}

