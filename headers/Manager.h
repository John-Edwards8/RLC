#pragma once
#include "Objects.h"
#include "Render/Renderer.h"
#include "Core/ISolver.h"
#include "Core/AdaptiveSolver.h"

class Manager {
	std::unique_ptr<Render::Display> _display;
	std::unique_ptr<Objects::Grid> _grid;
	std::unique_ptr<Core::ISolver> _solver;
	Render::SceneData _sceneData;

	int _screenWidth, _screenHeight;
	int _rows, _cols;
	int _cellSize;
	int _borderX, _borderY;
public:
	Manager(int impulse, int frequency, int w = 800, int h = 600, int r = 5, int c = 5);
	~Manager() = default;

	void updateScene();
	void step();
	void render();

	bool finished() const {
		return _solver->finished();
	}
};