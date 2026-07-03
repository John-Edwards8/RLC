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
	std::unique_ptr<Objects::Grid>   _grid;
	std::unique_ptr<Core::ISolver>   _solver;
	Render::SceneData				 _sceneData;
	Core::SolverType				 _solverType;
	std::array<Core::PlanSnapshot, 10> _allPlans;

	int _rows, _cols, _cellSize,
		_borderX, _borderY, _sweepBudget, _freq,
		_w, _h;
	double _sweepDuration;        // T обзора в секундах
	int _currentSweep = 0;        // номер текущего периода
	int _sweepImpulsesUsed = 0;   // потрачено в текущем периоде

	AppState _state = AppState::SETUP;

	std::vector<DetectionEvent> _detections;
	std::vector<bool>		    _alreadyDetected;

	// Replay
	std::vector<Render::FrameSnapshot> _history;
	int   _replayFrame = 0;
	float _replaySpeed = 1.0f;
	bool  _replayPaused = false;
	bool  _computeDone = false;

	// План текущего прогона (для визуализации)
	Core::PlanSnapshot _currentPlan;
	bool _showPlan = true; // показывать ли план поверх сетки

	void initSolver();
	void writeCSV();
	Render::FrameSnapshot buildSnapshot(int beamRow, int beamCol,
		int detRow, int detCol) const;
public:
	Manager(double duration, int frequency,
		int w = 800, int h = 600, int r = 5, int c = 5,
		Core::SolverType type = Core::SolverType::WEIGHTED_COEFFICIENTS);
	~Manager() = default;

	void updateScene();
	void render();
	void renderScene();
	void handleClick(int mouseX, int mouseY);
	void startSimulation();
	void compute();

	void resetReplay();
	void resetEndReplay();
	void replayTick();
	void replayStepForward();
	void replayStepBack();
	void togglePause() { _replayPaused = !_replayPaused; }
	void toggleShowPlan() { _showPlan = !_showPlan; }
	void speedUp();
	void speedDown();
	int  replayCurrentSweep() const;

	void reset(double duration, int frequency, int r, int c, Core::SolverType type);
	SDL_Window* getWindow() { return _display->getWindow(); }
	SDL_Renderer* getSDLRenderer() { return _display->getSDLRenderer(); }

	void setState(AppState s) {
		if (s == AppState::SETUP) _computeDone = false;
		_state = s;
		_sceneData.state = s;
	}
	AppState getState() const { return _state; }
	bool finished()     const { return _state == AppState::FINISHED; }
	int  replayFrame()  const { return _replayFrame; }
	int  historySize()  const { return static_cast<int>(_history.size()); }
	float replaySpeed()  const { return _replaySpeed; }
	bool replayPaused() const { return _replayPaused; }
	bool isComputeDone() const { return _computeDone; }
	bool isShowPlan()         const { return _showPlan; }
	const Core::PlanSnapshot& getCurrentPlan() const { return _currentPlan; }
	const Core::PlanSnapshot& getPlanForSweep(int sweep) const {
		int idx = sweep - 1;
		if (idx >= 0) return _allPlans[idx];
		return _currentPlan; // fallback
	}
	void setReplayFrame(int frame);
};

/*
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
		_borderX, _borderY, _sweepBudget, _freq,
		_w, _h;
	double _sweepDuration;        // T обзора в секундах
	int _currentSweep = 0;        // номер текущего периода
	int _sweepImpulsesUsed = 0;   // потрачено в текущем периоде

	AppState _state = AppState::SETUP;

	std::vector<DetectionEvent> _detections;
	std::vector<bool> _alreadyDetected;
	std::vector<std::vector<std::array<double, 3>>> _pdHistory;

	void initSolver();
	void writeCSV();
	void endSweep();
	void checkDetections();
	void finishSimulation();

public:
	Manager(double duration, int frequency,
			int w = 800, int h = 600, int r = 5, int c = 5,
			Core::SolverType type = Core::SolverType::WEIGHTED_COEFFICIENTS);
	~Manager() = default;

	void updateScene();
	void step();
	void render();
	void renderScene();
	void handleClick(int mouseX, int mouseY);
	void reset(double duration, int frequency, int r, int c, Core::SolverType type);
	SDL_Window* getWindow() { return _display->getWindow(); }
	SDL_Renderer* getSDLRenderer() { return _display->getSDLRenderer(); }
	bool running() const { return _state == AppState::RUNNING; }
	void startSimulation();

	bool finished() const {
		return _state == AppState::FINISHED;
	}
};
*/