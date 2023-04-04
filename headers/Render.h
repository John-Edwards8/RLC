#include "Window.h"

class Render: protected virtual Window {
protected:
	SDL_Renderer* rend;

	struct comp{
		int coordX;
		int coordY;
		unsigned int index;
		unsigned int targetChecker;
		bool target;
		// comp* next;
		// comp* prev;
	};

	/*struct GridList {
		comp* head;
		comp* tail;
	};*/


	/*void constr_list(GridList& l) {
		l.head = NULL;
	}

	inline bool chk_empty(GridList l) {
		return (l.head == NULL);
	}

	int struct_len(GridList& l) {
		if (chk_empty(l)) {
			return 0;
		}
		else {
			int count{ 0 }; comp* r = l.head;
			while (r->next != NULL) {
				r = r->next;
				count++;
			}
			count++;
			return count;
		}
	}

	void comp_in(GridList& l, int X, int Y) {
		comp* c = new comp();
		c->coordX = X;
		c->coordY = Y;
		c->target = rand() % 100 < 9? true:false;
		if (chk_empty(l)) {
			l.head = c;
			c->index = 1;
		}
		else {
			c->index = struct_len(l)+1;
			c->prev = l.tail;
			l.tail->next = c;
		}
		l.tail = c;
	}
*/

public:
	Render(){
		this->rend = SDL_CreateRenderer( getWindow(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	}

	void _render() {
		SDL_RenderPresent(this->rend);
	}
	
	void _clear() {
		SDL_SetRenderDrawColor( this->rend, 0xFF, 0xFF, 0xFF, 0xFF );
		SDL_RenderClear( this->rend );
	}
};