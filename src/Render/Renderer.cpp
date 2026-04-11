#include "Render/Renderer.h"

namespace Render {
	Display::Display() : Display(800, 600) {}

	Display::Display(unsigned width, unsigned height) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cout << "SDL failed to initialize! SDL error: " << SDL_GetError() << std::endl;
			exit(1);
		}

		_window = SDL_CreateWindow(
			"RLC",
			width,
			height,
			SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP
		);

		if (!_window) {
			std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
			exit(1);
		}

		_renderer = std::make_unique<StandardRenderer>(_window);

		SDL_ShowWindow(_window);
	}

	Display::~Display() {
		if (_window) {
			SDL_DestroyWindow(_window);
			_window = nullptr;
		}
		SDL_Quit();
	}

	StandardRenderer::StandardRenderer(SDL_Window* gWindow) {
		_rend = SDL_CreateRenderer(gWindow, nullptr);

		if (!_rend) {
			std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
			exit(1);
		}

		clear();
	}

	StandardRenderer::~StandardRenderer() {
		if (_rend) {
			SDL_DestroyRenderer(_rend);
			_rend = nullptr;
		}
	}

	void StandardRenderer::render(const SceneData& scene) {
		if (scene.cells.empty()) {
			std::cerr << "WARNING: Empty scene data!" << std::endl;
			return;
		}

		// Grid (black outlines)
		SDL_SetRenderDrawColor(_rend, 0x00, 0x00, 0x00, 0xFF);
		for (const auto& cell : scene.cells) {
			SDL_FRect rect = {
				cell.x,
				cell.y,
				cell.size,
				cell.size
			};
			SDL_RenderRect(_rend, &rect);
		}

		// Targets (red squares)
		SDL_SetRenderDrawColor(_rend, 0xFF, 0x00, 0x00, 0xFF);
		for (const auto& cell : scene.cells) {
			if (cell.confidence >= 0.9) {
				SDL_FRect rect = {
					cell.x + 5,
					cell.y + 5,
					cell.size - 10,
					cell.size - 10
				};
				SDL_RenderFillRect(_rend, &rect);
			}
		}

		// Beam (blue circle)
		if (scene.beamActive) {
			SDL_SetRenderDrawColor(_rend, 0x00, 0x00, 0xFF, 0xFF);

			int centerX = static_cast<int>(scene.beam.x);
			int centerY = static_cast<int>(scene.beam.y);
			int radius = static_cast<int>(scene.beam.radius);

			for (int w = -radius; w <= radius; w++) {
				for (int h = -radius; h <= radius; h++) {
					if ((w * w + h * h) <= (radius * radius)) {
						SDL_RenderPoint(_rend, centerX + w, centerY + h);
					}
				}
			}
		}
	}

	void StandardRenderer::present() {
		SDL_RenderPresent(_rend);
	}
	
	void StandardRenderer::clear() {
		SDL_SetRenderDrawColor(_rend, 0xFF, 0xFF, 0xFF, 0xFF); // White background
		SDL_RenderClear(_rend);
	}
}