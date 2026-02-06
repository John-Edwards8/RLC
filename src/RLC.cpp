#include "../headers/Manager.h"


int main( int argc, char* args[] ) {
	Manager* mainMan = new Manager();
	bool a;
	int frequency;
	int impulseCount;
	do{
		a = false;
		try{
			mainMan->setValues(static_cast<int&>(frequency), static_cast<int&>(impulseCount));
		}catch(const invalid_argument &ex){
			cout << ex.what() << endl;
			a = true;
		}
	} while(a);
	
	//ініціалізація сітки та вікна
	mainMan->initGrid();
	if( !mainMan->init() ) {
		cout << "Невдала ініціалізація!" << endl; 
	} else {
		//Прапорці
		bool quit = false;	//прапор виходу
		bool tik = false;	//прапор початку руху променя
		bool start = false; //прапор "першого циклу програми"

		//Оброблювач подій
		SDL_Event e;

		//Поки программа працює
		while( !quit ) {
			if (start) {
				mainMan->renderGrid();
				mainMan->_render();
			}
			if (tik){ 
				mainMan->moveBeam(impulseCount, frequency);
			}
			
			//Оброблюємо події з черги
			while( SDL_PollEvent( &e ) != 0 ) {
				if( e.type == SDL_EVENT_QUIT) {
					quit = true;
				} else if (e.type == SDL_EVENT_KEY_DOWN) {
					switch( e.key.scancode ) {
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

	//Звільнити ресурси та закрити SDL
	mainMan->close();

	return 0;
}