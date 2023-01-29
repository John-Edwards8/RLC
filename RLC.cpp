#include "headers/Manager.h"


int main( int argc, char* args[] ) {

	Manager* mainMan = new Manager;
	auto win = mainMan->getWindow();

	if( !win.init() )
	{
		cout << "Failed to initialize!" << endl;
	}
	else
	{
		//Main loop flag
		bool quit = false;

		//Event handler
		SDL_Event e;
		
		//While application is running
		while( !quit )
		{
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 )
			{
				//User requests quit
				if( e.type == SDL_QUIT )
				{
					quit = true;
				}
				else if (e.type == SDL_KEYDOWN) {
					win.handleKeys( e.key );
			    }	
			}
			mainMan->renderAll();
		}
	}

	//Free resources and close SDL
	win.close();

	return 0;
}