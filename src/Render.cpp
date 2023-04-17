#include "../headers/Render.h"


Render::Render(){
	this->rend = SDL_CreateRenderer( getWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
}

void Render::_render() {
	SDL_RenderPresent(this->rend);
}

void Render::_clear() {
	SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( this->rend );
}
