#include "../headers/Window.h"

void Window::init() {
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		cout << "SDL не зміг ініціалізуватися! Помилка SDL: " << SDL_GetError() << endl;
		exit(1);
	}

	gWindow = SDL_CreateWindow(
		"RLC",
		screenWidth,
		screenHeight,
		SDL_WINDOW_RESIZABLE
	);

	if (!gWindow) {
		cout << "Не вдалося створити вікно: " << SDL_GetError() << endl;
		exit(1);
	}

	rend = SDL_CreateRenderer(gWindow, nullptr);

	if (!rend) {
		cout << "Не вдалося створити renderer: " << SDL_GetError() << endl;
		exit(1);
	}
}

Window::Window() {
	screenWidth = 800;
	screenHeight = 600;
	init();
}

void Window::setValues(unsigned width, unsigned height) {
	screenWidth = width;
	screenHeight = height;

	SDL_SetWindowSize(gWindow, screenWidth, screenHeight);
}

void Window::close() {
	if (rend) {
		SDL_DestroyRenderer(rend);
		rend = nullptr;
	}

	if (gWindow) {
		SDL_DestroyWindow(gWindow);
		gWindow = nullptr;
	}

	SDL_Quit();
}

void Window::reCreate() {
	SDL_DestroyRenderer(rend);
	rend = SDL_CreateRenderer(gWindow, nullptr);
}

void Window::_render() {
	SDL_RenderPresent(rend);
}

//зафарбувати білим кольором
void Window::_clear() {
	SDL_SetRenderDrawColor(this->rend, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(this->rend);
}

