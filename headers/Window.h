#include "Prototypes.h"

class Window {
private:
	//Screen dimension
	int SCREEN_WIDTH = 1280;
	int SCREEN_HEIGHT = 720;

	//The window we'll be rendering to
	SDL_Window* gWindow;

	void initWindow() {
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
public:
	Window(){ initWindow(); }

	SDL_Window * getWindow() {
		return this->gWindow;
	}

	void setSize(int width, int height) {
		SCREEN_WIDTH = width > 0? width : SCREEN_WIDTH;
		SCREEN_HEIGHT = height > 0? height : SCREEN_HEIGHT;
	}

	bool init(){
		bool success = SDL_Init( SDL_INIT_VIDEO ) < 0? false : true;

		if( !success ) { cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl; }

		return success;
	}

	void handleKeys( auto key )	{
		switch( key.keysym.scancode ) {
		case SDL_SCANCODE_ESCAPE:
			close();
			exit(1);
		}
	}

	void close() { 
		//Destroy window	
		SDL_DestroyWindow( gWindow );
		gWindow = NULL;

		//Quit SDL subsystems
		SDL_Quit();
	}
};


