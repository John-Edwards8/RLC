#include "headers/Manager.h"


int main( int argc, char* args[] ) {
	Manager* mainMan = new Manager();
	bool a;
	do{
		a = false;
		try{
			mainMan->setValues();
		}catch(const invalid_argument &ex){
			cout << ex.what() << endl;
			a = true;
		}
	}while(a);
	
	mainMan->initGrid();

	if( !mainMan->init() ) {
		cout << "Failed to initialize!" << endl; 
	} else {
		//Flags
		bool quit = false;	//exit flag
		bool tik = false;	//moving the beam
		bool start = false; //start render of grid

		int frequency = 100;
		int impulseCount = 1000;
		
		//Event handler
		SDL_Event e;

		//While application is running
		while( !quit ) {
			if (start) {
				mainMan->renderGrid();
				mainMan->_render();
			}
			if (tik){ 
				mainMan->moveBeam(impulseCount, frequency);
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
						mainMan->close();
						exit(1);
					}
			    }	
			}
			start = true;
		}
	}

	//Free resources and close SDL
	mainMan->close();

	return 0;
}