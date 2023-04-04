#include "Prototypes.h"

class Window {
private:
	//Screen dimension
	int SCREEN_WIDTH;
	int SCREEN_HEIGHT;

	//The window we'll be rendering to
	SDL_Window* gWindow;

	bool tik = false;

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
	Window(int screenWidth, int screenHeight) {setSize(screenWidth, screenHeight); initWindow();}

	SDL_Window * getWindow() {
		return this->gWindow;
	}

	Window _getWindow() {
		return *this;
	}

	void setSize(int width, int height) {
		SCREEN_WIDTH = width > 0? width : SCREEN_WIDTH;
		SCREEN_HEIGHT = height > 0? height : SCREEN_HEIGHT;
	}

	int getWindowWidth() {
		return this->SCREEN_WIDTH;
	}

	int getWindowHeight() {
		return this->SCREEN_HEIGHT;
	}


	bool init(){
		bool success = SDL_Init( SDL_INIT_VIDEO ) < 0? false : true;

		if( !success ) { cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl; }

		return success;
	}

	bool getTik() {
		return this->tik;
	}

	void setTik(bool t) {
		this->tik = t;
	}

	void handleKeys( auto key )	{
		switch( key.keysym.scancode ) {
		case SDL_SCANCODE_SPACE:
			this->tik = this->tik? false: true;
			break;
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


