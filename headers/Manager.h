#pragma once
#include "Objects.h"
#include "Render/Renderer.h"
#include "Core/ISolver.h"
#include "Core/MaxElementSolver.h"
#include "Core/SequentialSolver.h"
#include "Core/TwoFunctionsSolver.h"
#include "Core/WeightedCoefficientsSolver.h"
#include <memory>
#include <limits>
#include <fstream>
#include <chrono>
#include <ctime>

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

	int _rows, _cols, _cellSize,
		_borderX, _borderY, _impulseLimit, _freq,
		_w, _h;

	AppState _state = AppState::SETUP;

	std::vector<DetectionEvent> _detections;
	std::vector<bool> _alreadyDetected;

	void initSolver();
	void writeCSV();
public:
	Manager(int impulse, int frequency,
			int w = 800, int h = 600, int r = 5, int c = 5,
		    Core::SolverType type = Core::SolverType::WEIGHTED_COEFFICIENTS);
	~Manager() = default;

	void updateScene();
	void step();
	void render();
	void renderScene();
	void handleClick(int mouseX, int mouseY);
	void reset(int impulse, int frequency, int r, int c, Core::SolverType type);
	SDL_Window* getWindow() { return _display->getWindow(); }
	SDL_Renderer* getSDLRenderer() { return _display->getSDLRenderer(); }
	bool running() const { return _state == AppState::RUNNING; }
	void startSimulation();

	bool finished() const {
		return _state == AppState::FINISHED;
	}
};