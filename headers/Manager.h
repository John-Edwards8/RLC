#pragma once
#include "Objects.h"
#include "Render/Renderer.h"
#include "Core/ISolver.h"
#include "Core/MaxElementSolver.h"
#include "Core/SequentialSolver.h"
#include <memory>

class Manager {
	std::unique_ptr<Render::Display> _display;
	std::unique_ptr<Objects::Grid> _grid;
	std::unique_ptr<Core::ISolver> _solver;
	Render::SceneData _sceneData;
	Core::SolverType _solverType;

	int _screenWidth, _screenHeight;
	int _rows, _cols;
	int _cellSize;
	int _borderX, _borderY;

	void updateBelief(Objects::Cell& cell, double signal);
public:
	Manager(int impulse, int frequency,
			int w = 800, int h = 600, int r = 5, int c = 5,
		    Core::SolverType type = Core::SolverType::MAX_ELEMENT);
	~Manager() = default;

	void updateScene();
	void step();
	void render();

	bool finished() const {
		return _solver->finished();
	}
};