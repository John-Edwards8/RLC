#include "Window.h"

class Render : virtual protected Window {
protected:
	SDL_Renderer* rend;
public:
	Render(){
		this->rend = SDL_CreateRenderer( getWindow(), -1, SDL_RENDERER_ACCELERATED );
		SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( this->rend );
	}
	void _render() {
		SDL_RenderPresent(this->rend);
	}	
};