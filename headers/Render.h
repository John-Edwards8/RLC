#include "Window.h"

class Render: protected virtual Window {
protected:
	SDL_Renderer* rend;
public:
	Render(){
		this->rend = SDL_CreateRenderer( getWindow(), -1, SDL_RENDERER_ACCELERATED );
	}
	// Render(int screenWidth, int screenHeight) : Render() {}

	void _render() {
		SDL_RenderPresent(this->rend);
	}
	
	void _clear() {
		SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( this->rend );
	}	

};