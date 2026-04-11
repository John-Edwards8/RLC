#pragma	once
#include "Objects.h"

#include "RenderData.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>

namespace Render {
	class Renderer {
	public:
		virtual void render(const SceneData& scene) = 0;
		virtual void present() = 0;
		virtual void clear() = 0;
		virtual ~Renderer() = default;
	};

	class StandardRenderer : public Renderer {
		SDL_Renderer* _rend;
	public:
		StandardRenderer(SDL_Window* gWindow);

		void render(const SceneData& scene) override;
		void present() override;
		void clear();

		~StandardRenderer();
	};

	class Display {
		SDL_Window* _window;
		std::unique_ptr<Renderer> _renderer;
	public:
		Display();
		Display(unsigned width, unsigned height);

		Renderer* getRenderer() { return _renderer.get(); }

		~Display();
	};	
}