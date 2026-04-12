#pragma once
#include "Objects.h"
#include "Render/Renderer.h"
#include "Core/ISolver.h"
#include "Core/MaxElementSolver.h"
#include "Core/SequentialSolver.h"
#include <memory>

struct DetectionEvent {
	int row, col;
	int impulsesAtDetection;
	bool isCorrect;
};

class Manager {
	std::unique_ptr<Render::Display> _display;
	std::unique_ptr<Objects::Grid> _grid;
	std::unique_ptr<Core::ISolver> _solver;
	Render::SceneData _sceneData;
	Core::SolverType _solverType;

	int _screenWidth, _screenHeight,
		_rows, _cols, _cellSize,
		_borderX, _borderY, _impulseLimit;

	AppState _state = AppState::SETUP;

	std::vector<DetectionEvent> _detections;
	std::vector<bool> _alreadyDetected;

	void initSolver();
public:
	Manager(int impulse, int frequency,
			int w = 800, int h = 600, int r = 5, int c = 5,
		    Core::SolverType type = Core::SolverType::MAX_ELEMENT);
	~Manager() = default;

	void updateScene();
	void step();
	void render();
	void handleClick(int mouseX, int mouseY);
	void startSimulation();

	bool finished() const {
		return _state == AppState::FINISHED;
	}
};