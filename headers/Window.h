#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>
using namespace std;

#include "Prototypes.h"

class Window{

private:
	//Screen dimension
	int SCREEN_WIDTH = 640;
	int SCREEN_HEIGHT = 480;

	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//OpenGL context
	SDL_GLContext gContext;

	//Render flag
	bool gRenderQuad = true;

public:
	Window(){}
	Window(int width, int height){ setSize(width, height); }

	SDL_Window * getWindow() {
		return gWindow;
	}

	void setSize(int width, int height) {
		SCREEN_WIDTH = width > 0? width : SCREEN_WIDTH;
		SCREEN_HEIGHT = height > 0? height : SCREEN_HEIGHT;
	}

	bool init()
	{
		//Initialization flag
		bool success = true;

		//Initialize SDL
		if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		{
			cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
			success = false;
		}
		else
		{
			//Use OpenGL 2.1
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

			//Create window
			gWindow = SDL_CreateWindow( "RLC", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
			if( gWindow == NULL )
			{
				cout << "Window could not be created! SDL Error: " << SDL_GetError() << endl;
				success = false;
			}
			else
			{
				//Create context
				gContext = SDL_GL_CreateContext( gWindow );
				if( gContext == NULL )
				{
					cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << endl;
					success = false;
				}
				else
				{
					//Use Vsync
					if( SDL_GL_SetSwapInterval( 1 ) < 0 )
					{
						cout << "Warning: Unable to set VSync! SDL Error: " << SDL_GetError() << endl;
					}

					//Initialize OpenGL
					if( !initGL() )
					{
						cout << "Unable to initialize OpenGL!" << endl;
						success = false;
					}
				}
			}
		}

		return success;
	}

	bool errorCheckerGL(bool success){
		GLenum error = glGetError();
		if( error != GL_NO_ERROR )
		{
			cout << "Error initializing OpenGL! " << error << endl;
			success = false;
		}
		return success;
	}

	bool initGL()
	{
		bool success = true;

		//Initialize Projection Matrix
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		
		//Check for error
		success = errorCheckerGL(success);

		//Initialize Modelview Matrix
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		success = errorCheckerGL(success);

		//Initialize clear color
		glClearColor( 0.f, 0.f, 0.f, 1.f );

		success = errorCheckerGL(success);

		return success;
	}

	void handleKeys( unsigned char key, int x, int y )
	{
		//Toggle quad
		if( key == 'q' )
		{
			gRenderQuad = !gRenderQuad;
		}
	}

	void update()
	{
		//No per frame update needed
	}

	void render()
	{
		//Clear color buffer
		glClear( GL_COLOR_BUFFER_BIT );
		
		//Render quad
		if( gRenderQuad )
		{
			glBegin( GL_QUADS );
				glVertex2f( -0.5f, -0.5f );
				glVertex2f( 0.5f, -0.5f );
				glVertex2f( 0.5f, 0.5f );
				glVertex2f( -0.5f, 0.5f );
			glEnd();
		}
	}

	void close()
	{
		//Destroy window	
		SDL_DestroyWindow( gWindow );
		gWindow = NULL;

		//Quit SDL subsystems
		SDL_Quit();
	}
};


