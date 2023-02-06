#include "headers/Manager.h"


int main( int argc, char* args[] ) {

	Manager* mainMan = new Manager(1360, 768, 30, 5);
	auto win = mainMan->getWindow();
	mainMan->start();

	if( !win.init() )
	{
		cout << "Failed to initialize!" << endl;
	}
	else
	{
		//Main loop flag
		bool quit = false;

		bool start = false;
		
		//Event handler
		SDL_Event e;

		//While application is running
		while( !quit ) {
			if (start) { mainMan->clear(); ((Render*)mainMan)->_render(); }

			if (win.getTik()){
				mainMan->moveBeam();
			}
			
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 ) {
				//User requests quit
				if( e.type == SDL_QUIT ) {
					quit = true;
				}
				else if (e.type == SDL_KEYDOWN) {
					win.handleKeys( e.key );
			    }	
			}
			start = true;
		}
	}

	//Free resources and close SDL
	win.close();

	return 0;
}