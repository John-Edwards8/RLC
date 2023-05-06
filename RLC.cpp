#include "headers/Manager.h"


int main( int argc, char* args[] ) {

	Manager* mainMan = new Manager(1360, 768, 5, 3);
	auto win = mainMan->getWindow();
	mainMan->initGrid();

	if( !win.init() ) {
		cout << "Failed to initialize!" << endl; 
	} else {
		//Flags
		bool quit = false;	//exit flag
		bool tik = false;	//moving the beam
		bool start = false; //start render of grid

		int stage = 0;
		int impulseCount = 1000;
		
		//Event handler
		SDL_Event e;

		//While application is running
		while( !quit ) {
			if (start) { if(stage <= 1) { mainMan->clear(); ((Render*)mainMan)->_render(); } }
			if (tik){ 
				if(stage<=1) { mainMan->moveBeam(stage, impulseCount); stage++; }
			}
			
			//Handle events on queue
			while( SDL_PollEvent( &e ) != 0 ) {
				//User requests quit
				if( e.type == SDL_QUIT ) {
					quit = true;
				} else if (e.type == SDL_KEYDOWN) {
					switch( e.key.keysym.scancode ) {
					case SDL_SCANCODE_SPACE:
						tik = tik? false: true;
						break;
					case SDL_SCANCODE_ESCAPE:
						win.close();
						exit(1);
					}
			    }	
			}
			start = true;
		}
	}

	//Free resources and close SDL
	win.close();

	return 0;
}