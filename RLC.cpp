#include <iostream>
#include <string>
#include "headers/Window.h"


int main( int argc, char* args[] )
{
	Window win(1280, 720);
	//Start up SDL and create window
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
		
		//Enable text input
		//SDL_StartTextInput();

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
				//Handle keypress with current mouse position
				/*else if( e.type == SDL_TEXTINPUT )
				{
					int x = 0, y = 0;
					SDL_GetMouseState( &x, &y );
					win.handleKeys( e.text.text[ 0 ], x, y );
				}*/
			}

			//Render quad
			(win.getRender()).render();
			
			//Update screen
			SDL_GL_SwapWindow( win.getWindow() );
		}
		
		//Disable text input
		//SDL_StopTextInput();
	}

	//Free resources and close SDL
	win.close();

	return 0;
}