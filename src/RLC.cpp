#include "../headers/Manager.h"

int main( int argc, char* args[] ) {
	int w, h, r, c, frequency = 100, impulse = 10000;
	/*
	do{
		a = false;
		try{
			
			std::cout << "Enter the window width:" << std::endl;
			if (!(std::cin >> w)) {
				std::cin.clear();
				while (std::cin.get() != '\n') continue;
				throw std::invalid_argument("The width entered is incorrect!");
			}
			std::cout << "Enter the window height:" << std::endl;
			if (!(std::cin >> h)) {
				std::cin.clear();
				while (std::cin.get() != '\n') continue;
				throw std::invalid_argument("Incorrect height entered!");
			}
			std::cout << "Enter the grid cell values ​​(how many in a row and in a column):" << std::endl;
			if (!(std::cin >> r >> c)) {
				std::cin.clear();
				while (std::cin.get() != '\n') continue;
				throw std::invalid_argument("Incorrect values ​​entered!");
			}
			
			std::cout << "Enter the total number of pulses:" << std::endl;
			if (!(std::cin >> impulse)) {
				std::cin.clear();
				while (std::cin.get() != '\n') continue;
				throw std::invalid_argument("Incorrect quantity entered!");
			}
			std::cout << "Enter the pulse frequency:" << std::endl;
			if (!(std::cin >> frequency)) {
				std::cin.clear();
				while (std::cin.get() != '\n') continue;
				throw std::invalid_argument("Incorrect frequency entered!");
			}
		}catch(const std::invalid_argument &ex){
			std::cout << ex.what() << std::endl;
			a = true;
		}
	} while(a);
	TODO: UI via Dear ImGui*/

	std::unique_ptr<Manager> mainMan = std::make_unique<Manager>(impulse, frequency, 1280, 720, 5, 5);

	bool quit = false;
	bool paused = true;

	SDL_Event e;

	while (!quit) {
		if (!paused && !mainMan->finished()) {
			mainMan->step();
			SDL_Delay(50);
		}

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			else if (e.type == SDL_EVENT_KEY_DOWN) {
				switch (e.key.scancode) {
				case SDL_SCANCODE_SPACE:
					paused = paused ? false : true;
					break;
				case SDL_SCANCODE_ESCAPE:
					exit(1);
				}
			}			
		}

		if (mainMan->finished()) paused = paused ? false : true;
	}

	return 0;
}