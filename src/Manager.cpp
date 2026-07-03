/*
#include "../headers/Manager.h"

void Manager::initSolver() {
	std::fill(_alreadyDetected.begin(), _alreadyDetected.end(), false);
	_detections.clear();
	_pdHistory.assign(_rows,
		std::vector<std::array<double, 3>>(_cols, { 0.0, 0.0, 0.0 }));
	_currentSweep = 0;
	_sweepImpulsesUsed = 0;

	switch (_solverType) {
	case Core::SolverType::SEQUENTIAL:
		_solver = std::make_unique<Core::SequentialSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::MAX_ELEMENT:
		_solver = std::make_unique<Core::MaxElementSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::TWO_FUNCTIONS:
		_solver = std::make_unique<Core::TwoFunctionsSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::WEIGHTED_COEFFICIENTS:
		_solver = std::make_unique<Core::WeightedCoefficientsSolver>(
			_rows, _cols, _grid->model);
		break;
	}
}

void Manager::writeCSV() {
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	char buf[20];
	std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&t));

	const std::string algo = (_solverType == Core::SolverType::SEQUENTIAL)
		? "SEQUENTIAL" : "MAX_ELEMENT";

	std::string filename = std::string(buf) + "_" + algo + ".csv";
	std::ofstream f(filename);
	f << "algo,row,col,impulses_at_detection,t_detect_sec,is_correct\n";
	for (auto& d : _detections) {
		f << algo << ","
			<< d.row + 1 << ","
			<< d.col + 1 << ","
			<< d.impulsesAtDetection << ","
			<< static_cast<double>(d.impulsesAtDetection) / _freq << ","
			<< (d.isCorrect ? 1 : 0) << "\n";
	}
}

void Manager::endSweep() {
	// 1. Нормализовать belief
	_solver->normalizeBeliefs();

	// 2. Сохранить Pd для каждой клетки в историю
	double p = _solver->effectiveDetectProb();
	int slot = _currentSweep % 3;
	for (int r = 0; r < _rows; r++)
		for (int c = 0; c < _cols; c++) {
			int n = _solver->getN(r, c);  // нужен геттер _n
			_pdHistory[r][c][slot] = 1.0 - std::pow(1.0 - p, n);
		}

	// 3. Проверить правило 2-из-3 начиная со второго периода
	if (_currentSweep >= 2) {
		checkDetections();
	}

	// 4. Проверить остановку — все цели найдены?
	int found = 0;
	for (auto& d : _detections)
		if (d.isCorrect) found++;
	if (found >= _grid->getTargetCount()) {
		finishSimulation();
		return;
	}

	// 5. Начать новый период
	_currentSweep++;
	_sweepImpulsesUsed = 0;
	_solver->resetSweepImpulses();

	std::cout << "\n--- SWEEP " << _currentSweep << " ---\n";
}

void Manager::checkDetections() {
	double p = _solver->effectiveDetectProb();
	constexpr double PD_THRESHOLD = 0.5;

	for (int r = 0; r < _rows; r++) {
		for (int c = 0; c < _cols; c++) {
			int idx = r * _cols + c;
			if (_alreadyDetected[idx]) continue;

			// Считаем сколько из последних 3 периодов Pd >= порога
			int hits = 0;
			for (int k = 0; k < 3; k++)
				if (_pdHistory[r][c][k] >= PD_THRESHOLD) hits++;

			if (hits >= 2) {
				_alreadyDetected[idx] = true;
				bool correct = _grid->coords[r][c].realTarget;
				_detections.push_back({ r, c, _solver->getTotalImpulses(), correct });
				_solver->markDecided(r, c);
				std::cout << (correct ? "TARGET" : "FALSE POS")
					<< " detected at (" << r + 1 << "," << c + 1
					<< ") after sweep " << _currentSweep << "\n";
			}
		}
	}
}

void Manager::finishSimulation() {
	_state = AppState::FINISHED;
	_sceneData.state = AppState::FINISHED;
}

Manager::Manager(double duration, int frequency,
				 int w, int h, int r, int c,
				 Core::SolverType type) :
	_rows(r), _cols(c), _solverType(type), _sweepDuration(duration), _freq(frequency),
	_display(std::make_unique<Render::Display>(w, h)),
	_grid(std::make_unique<Objects::Grid>(r, c)),
	_alreadyDetected(r* c, false),
	_w(w), _h(h)
{
	_sweepBudget = static_cast<int>(duration * frequency);
	int maxDim = std::max(_rows, _cols);
	_cellSize = (_h * 9 / 10) / maxDim;
	_borderX = (_w - _cellSize * _cols) / 2;
	_borderY = (_h - _cellSize * _rows) / 2;

	_sceneData.rows = _rows;
	_sceneData.cols = _cols;
	_sceneData.cellSize = _cellSize;
	_sceneData.borderX = _borderX;
	_sceneData.borderY = _borderY;
	_sceneData.beam.radius = _cellSize / 4.0f;
	_sceneData.beam.x = _borderX + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + _cellSize / 2.0f;
	_sceneData.state = AppState::SETUP;

	updateScene();
	render();	
}

void Manager::updateScene() {
	_sceneData.cells.clear();
	_sceneData.cells.reserve(_rows * _cols);

	for (int row = 0; row < _rows; ++row) {
		for (int col = 0; col < _cols; ++col) {
			const auto& gridCell = _grid->coords[row][col];

			Render::CellRenderInfo cellInfo;
			cellInfo.x = static_cast<float>(_borderX + col * _cellSize);
			cellInfo.y = static_cast<float>(_borderY + row * _cellSize);
			cellInfo.size = static_cast<float>(_cellSize);
			cellInfo.isTarget = gridCell.realTarget;
			cellInfo.confidence = (_solver != nullptr)
				? _solver->getBelief(row, col)
				: 0.0;
			_sceneData.cells.push_back(cellInfo);
		}
	}
}

void Manager::step() {
	if (_state != AppState::RUNNING || !_solver) return;
	if (_sweepImpulsesUsed >= _sweepBudget) {
		endSweep();
		return;
	}

	auto [row, col] = _solver->chooseCell();
	auto& cell = _grid->coords[row][col];
	double signal = _grid->measure(row, col);
	_solver->onSignalResult(row, col, signal);
	_sweepImpulsesUsed++;

	_sceneData.beam.x = _borderX + col * _cellSize + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + row * _cellSize + _cellSize / 2.0f;

	updateScene();
	std::cout << "SweepBudget: " << _sweepBudget
		<< " Duration: " << _sweepDuration
		<< " Freq: " << _freq << "\n";
	
	


	//int idx = row * _cols + col;
	//double belief = _solver->getBelief(row, col);
	//
	//if (_solver->getRecentPositives(row, col) >= 2
	//	&& belief >= 0.7
	//	&& !_alreadyDetected[idx])
	//{
	//	_alreadyDetected[idx] = true;
	//	_detections.push_back({ row, col, _solver->getTotalImpulses(), _grid->coords[row][col].realTarget });
	//	_solver->markDecided(row, col);
	//	std::cout << "TARGET DETECTED at (" << row + 1 << "," << col + 1
	//		<< ") after " << _solver->getTotalImpulses() << " impulses\n";
	//}

	//if (_solver->finished()) {
	//	_state = AppState::FINISHED;
	//	_sceneData.state = AppState::FINISHED;
	//	std::cout << "\n=== SEARCH COMPLETE ===" << std::endl;

	//	int found = 0;
	//	for (auto& d : _detections) {
	//		bool correct = _grid->coords[d.row][d.col].realTarget;
	//		std::cout << "  (" << d.row + 1 << "," << d.col + 1 << ") "
	//			<< (correct ? "CORRECT" : "FALSE POSITIVE")
	//			<< " - impulses: " << d.impulsesAtDetection << "\n";
	//		if (correct) found++;
	//	}
	//	std::cout << "Total found: " << found << " of " << _grid->getTargetCount() << std::endl;

	//	if (!_detections.empty()) {
	//		double tMin = std::numeric_limits<double>::max();
	//		double tMax = 0.0;
	//		double tSum = 0.0;
	//		for (auto& d : _detections) {
	//			double t = static_cast<double>(d.impulsesAtDetection) / _freq;
	//			tMin = std::min(tMin, t);
	//			tMax = std::max(tMax, t);
	//			tSum += t;
	//		}
	//		double tAvg = tSum / _detections.size();
	//		std::cout << "T_detect (sec): min=" << tMin
	//			<< " avg=" << tAvg
	//			<< " max=" << tMax
	//			<< " (freq=" << _freq << " Hz)\n";
	//	}
	//	writeCSV();
	//	std::cout << "Results saved to results.csv\n";
	//	return;
	}
	
}

void Manager::renderScene() {
	_display->getRenderer()->clear();
	_display->getRenderer()->render(_sceneData);
}

void Manager::render() {
	_display->getRenderer()->clear();
	_display->getRenderer()->render(_sceneData);
	_display->getRenderer()->present();
}

void Manager::handleClick(int mouseX, int mouseY) {
	if (_state != AppState::SETUP) return;

	int col = (mouseX - _borderX) / _cellSize;
	int row = (mouseY - _borderY) / _cellSize;

	if (row < 0 || row >= _rows || col < 0 || col >= _cols) return;

	_grid->toggleTarget(row, col);
	updateScene();
	render();
}

void Manager::reset(double duration, int frequency, int r, int c, Core::SolverType type) {
	_sweepDuration = duration;
	_freq = frequency;
	_sweepBudget = static_cast<int>(duration * frequency);
	_solverType = type;
	_rows = r; _cols = c;
	_state = AppState::SETUP;
	_sceneData.state = AppState::SETUP;

	_solver.reset();
	_detections.clear();
	_grid = std::make_unique<Objects::Grid>(r, c);
	_alreadyDetected.assign(r * c, false);

	int maxDim = std::max(r, c);
	_cellSize = (_h * 9 / 10) / maxDim;
	_borderX = (_w - _cellSize * c) / 2;
	_borderY = (_h - _cellSize * r) / 2;

	_sceneData.rows = r;
	_sceneData.cols = c;
	_sceneData.cellSize = _cellSize;
	_sceneData.borderX = _borderX;
	_sceneData.borderY = _borderY;
	_sceneData.beam.radius = _cellSize / 4.0f;
	_sceneData.beam.x = _borderX + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + _cellSize / 2.0f;

	updateScene();
	render();
}

void Manager::startSimulation() {
	if (_state != AppState::SETUP) return;
	if (_grid->getTargetCount() == 0) {
		std::cout << "Place at least one target before starting!\n";
		return;
	}
	
	initSolver();
	_state = AppState::RUNNING;
	_sceneData.state = AppState::RUNNING;
	std::cout << "Simulation started. Targets: "
		<< _grid->getTargetCount() << "\n";

	std::cout << "SweepBudget: " << _sweepBudget
		<< " Duration: " << _sweepDuration
		<< " Freq: " << _freq << "\n";
}
*/

#include "../headers/Manager.h"

void Manager::initSolver() {
	std::fill(_alreadyDetected.begin(), _alreadyDetected.end(), false);
	_detections.clear();

	switch (_solverType) {
	case Core::SolverType::SEQUENTIAL:
		_solver = std::make_unique<Core::SequentialSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::MAX_ELEMENT:
		_solver = std::make_unique<Core::MaxElementSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::TWO_FUNCTIONS:
		_solver = std::make_unique<Core::TwoFunctionsSolver>(
			_rows, _cols, _grid->model);
		break;
	case Core::SolverType::WEIGHTED_COEFFICIENTS:
		_solver = std::make_unique<Core::WeightedCoefficientsSolver>(
			_rows, _cols, _grid->model);
		break;
	}
}

void Manager::writeCSV() {
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	char buf[20];
	std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", std::localtime(&t));

	const std::string algo = (_solverType == Core::SolverType::SEQUENTIAL)
		? "SEQUENTIAL" : (_solverType == Core::SolverType::MAX_ELEMENT)
		? "MAX_ELEMENT" : (_solverType == Core::SolverType::TWO_FUNCTIONS)
		? "TWO_FUNCTIONS" : "WEIGHTED_COEFFICIENTS";

	std::string filename = std::string(buf) + "_" + algo + ".csv";
	std::ofstream f(filename);
	f << "algo,row,col,impulses_at_detection,t_detect_sec,is_correct\n";
	for (auto& d : _detections) {
		f << algo << ","
			<< d.row + 1 << ","
			<< d.col + 1 << ","
			<< d.impulsesAtDetection << ","
			<< static_cast<double>(d.impulsesAtDetection) / _freq << ","
			<< (d.isCorrect ? 1 : 0) << "\n";
	}
}

Manager::Manager(double duration, int frequency,
	int w, int h, int r, int c,
	Core::SolverType type) :
	_rows(r), _cols(c), _solverType(type), _sweepDuration(duration), _freq(frequency),
	_display(std::make_unique<Render::Display>(w, h)),
	_grid(std::make_unique<Objects::Grid>(r, c)),
	_alreadyDetected(r* c, false),
	_w(w), _h(h)
{
	_sweepBudget = static_cast<int>(duration * frequency);
	int viewW = _w - 320;
	int viewH = _h - 100 - 24;

	int maxDim = std::max(_rows, _cols);
	_cellSize = (std::min(viewW, viewH) * 9 / 10) / maxDim;
	_borderX = (viewW - _cellSize * _cols) / 2;
	_borderY = 24 + (viewH - _cellSize * _rows) / 2;

	_sceneData.rows = _rows;
	_sceneData.cols = _cols;
	_sceneData.cellSize = _cellSize;
	_sceneData.borderX = _borderX;
	_sceneData.borderY = _borderY;
	_sceneData.beam.radius = _cellSize / 4.0f;
	_sceneData.beam.x = _borderX + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + _cellSize / 2.0f;
	_sceneData.state = AppState::SETUP;

	updateScene();
	render();
}

void Manager::updateScene() {
	_sceneData.cells.clear();
	_sceneData.cells.reserve(_rows * _cols);

	for (int row = 0; row < _rows; ++row) {
		for (int col = 0; col < _cols; ++col) {
			const auto& gridCell = _grid->coords[row][col];

			Render::CellRenderInfo cellInfo;
			cellInfo.x = static_cast<float>(_borderX + col * _cellSize);
			cellInfo.y = static_cast<float>(_borderY + row * _cellSize);
			cellInfo.size = static_cast<float>(_cellSize);
			cellInfo.isTarget = gridCell.realTarget;
			cellInfo.confidence = (_solver != nullptr)
				? _solver->getBelief(row, col)
				: 0.0;

			if (_showPlan && _currentPlan.allocations.size() == (size_t)_rows) {
				cellInfo.planAllocation = _currentPlan.allocations[row][col];
				cellInfo.planVisitOrder = _currentPlan.visitOrder[row][col];
			}
			else {
				cellInfo.planAllocation = 0;
				cellInfo.planVisitOrder = 0;
			}
			_sceneData.cells.push_back(cellInfo);
		}
	}
}

void Manager::renderScene() {
	_display->getRenderer()->clear();
	_display->getRenderer()->render(_sceneData);
}

void Manager::render() {
	_display->getRenderer()->clear();
	_display->getRenderer()->render(_sceneData);
	_display->getRenderer()->present();
}

void Manager::handleClick(int mouseX, int mouseY) {
	if (_state != AppState::SETUP) return;

	int col = (mouseX - _borderX) / _cellSize;
	int row = (mouseY - _borderY) / _cellSize;

	if (row < 0 || row >= _rows || col < 0 || col >= _cols) return;

	_grid->toggleTarget(row, col);
	updateScene();
}

void Manager::reset(double duration, int frequency, int r, int c, Core::SolverType type) {
	_computeDone = false;
	_sweepDuration = duration;
	_freq = frequency;
	_sweepBudget = static_cast<int>(duration * frequency);
	_solverType = type;
	_rows = r; _cols = c;
	_state = AppState::SETUP;
	_sceneData.state = AppState::SETUP;

	_solver.reset();
	_detections.clear();
	_grid = std::make_unique<Objects::Grid>(r, c);
	_alreadyDetected.assign(r * c, false);
	_currentPlan = Core::PlanSnapshot{};

	int viewW = _w - 320;
	int viewH = _h - 100 - 24;

	int maxDim = std::max(_rows, _cols);
	_cellSize = (std::min(viewW, viewH) * 9 / 10) / maxDim;
	_borderX = (viewW - _cellSize * _cols) / 2;
	_borderY = 24 + (viewH - _cellSize * _rows) / 2;

	_sceneData.rows = r;
	_sceneData.cols = c;
	_sceneData.cellSize = _cellSize;
	_sceneData.borderX = _borderX;
	_sceneData.borderY = _borderY;
	_sceneData.beam.radius = _cellSize / 4.0f;
	_sceneData.beam.x = _borderX + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + _cellSize / 2.0f;

	updateScene();
}

void Manager::setReplayFrame(int frame) {
	if (frame >= 0 && frame < (int)_history.size()) {
		_replayFrame = frame;
		const auto& snap = _history[_replayFrame];
		_sceneData.cells = snap.cells;
		_sceneData.beam = snap.beam;
	}
}

void Manager::startSimulation() {
	if (_grid->getTargetCount() == 0) {
		std::cout << "Place at least one target before starting!\n";
		_state = AppState::SETUP;
		return;
	}
	initSolver();
	_state = AppState::COMPUTING;
	std::cout << "Computing... Targets: " << _grid->getTargetCount();
	compute();
}

Render::FrameSnapshot Manager::buildSnapshot(int beamRow, int beamCol,
	int detRow, int detCol) const {
	Render::FrameSnapshot snap;
	snap.sweepNumber = _solver->getLastPlan().sweepNumber;
	snap.beam.x = _borderX + beamCol * _cellSize + _cellSize / 2.0f;
	snap.beam.y = _borderY + beamRow * _cellSize + _cellSize / 2.0f;
	snap.beam.radius = _cellSize / 4.0f;
	snap.impulseIndex = _solver->getTotalImpulses();
	snap.detectedRow = detRow;
	snap.detectedCol = detCol;

	snap.cells.reserve(_rows * _cols);
	for (int r = 0; r < _rows; r++) {
		for (int c = 0; c < _cols; c++) {
			Render::CellRenderInfo ci;
			ci.x = static_cast<float>(_borderX + c * _cellSize);
			ci.y = static_cast<float>(_borderY + r * _cellSize);
			ci.size = static_cast<float>(_cellSize);
			ci.isTarget = _grid->coords[r][c].realTarget;
			ci.confidence = _solver->getBelief(r, c);
			ci.isRevealed = _alreadyDetected[r * _cols + c] && _grid->coords[r][c].realTarget;
			ci.isDetected = _alreadyDetected[r * _cols + c];
			if (_showPlan && _currentPlan.allocations.size() == (size_t)_rows) {
				ci.planAllocation = _currentPlan.allocations[r][c];
				ci.planVisitOrder = _currentPlan.visitOrder[r][c];
				ci.planVisitRank = _currentPlan.visitRank[r][c];
			}
			else {
				ci.planAllocation = 0;
				ci.planVisitOrder = 0;
				ci.planVisitRank = 0;
			}
			snap.cells.push_back(ci);
		}
	}
	return snap;
}

void Manager::compute() {
	_history.clear();
	_history.reserve(_rows * _cols);

	int lastLoggedSweep = -1;

	while (!_solver->finished()) {
		auto [row, col] = _solver->chooseCell(_sweepBudget);

		// Обновить отображаемый план когда начался новый прогон
		int planSweep = _solver->getLastPlan().sweepNumber;
		if (planSweep != lastLoggedSweep) {
			lastLoggedSweep = planSweep;
			_currentPlan = _solver->getLastPlan();
			int si = _currentPlan.sweepNumber - 1;
			if (si >= 0)
				_allPlans[si] = _currentPlan;
			std::cout << "\n--- SWEEP " << si << " ---\n";
			std::cout << "Plan allocations (impulses per cell):\n";
			for (int r = 0; r < _rows; r++) {
				for (int c = 0; c < _cols; c++)
					std::cout << _currentPlan.allocations[r][c] << "\t";
				std::cout << "\n";
			}
		}

		double signal = _grid->measure(row, col);
		_solver->onSignalResult(row, col, signal);
		_history.push_back(buildSnapshot(row, col, -1, -1));

		// Критерий обнаружения: 2 из 3 последних + belief >= 0.9
		int idx = row * _cols + col;
		if (_solver->getRecentPositives(row, col) >= 2
			&& _solver->getBelief(row, col) >= 0.9
			&& !_alreadyDetected[idx])
		{
			_alreadyDetected[idx] = true;
			bool correct = _grid->coords[row][col].realTarget;
			_detections.push_back({ row, col, _solver->getTotalImpulses(), correct });
			_solver->markDecided(row, col);
			std::cout << (correct ? "TARGET" : "FALSE POS") << " detected at ("
				<< row + 1 << "," << col + 1 << ") after "
				<< _solver->getTotalImpulses() << " impulses\n";
		}
	}

	_solver->flushPending();

	// Summary
	std::cout << "\n=== SEARCH COMPLETE ===\n";
	int found = 0, lastImpulse = 0;
	for (auto& d : _detections) {
		std::cout << "  (" << d.row + 1 << "," << d.col + 1 << ") "
			<< (d.isCorrect ? "CORRECT" : "FALSE POSITIVE")
			<< " - impulses: " << d.impulsesAtDetection << "\n";
		if (d.isCorrect) {
			found++;
			lastImpulse = std::max(lastImpulse, d.impulsesAtDetection);
		}
	}
	std::cout << "Total found: " << found << " of " << _grid->getTargetCount() << "\n";
	std::cout << "T_search: " << lastImpulse << " impulses = "
		<< (double)lastImpulse / _freq << " sec\n";

	writeCSV();
	_computeDone = true;
	_state = AppState::REPLAY;
	_replayFrame = 0;
	_replayPaused = true;
	if (!_history.empty()) {
		_sceneData.cells = _history[0].cells;
		_sceneData.beam = _history[0].beam;
		_sceneData.state = AppState::REPLAY;
		_replayFrame = 1;
	}
	std::cout << "Ready for replay. Frames: " << _history.size() << "\n";
}

void Manager::speedUp() {
	if (_replaySpeed < 0.5f) _replaySpeed += 0.25f;
	else if (_replaySpeed < 2.0f) _replaySpeed += 0.5f;
}

void Manager::speedDown() {
	if (_replaySpeed > 0.5f) _replaySpeed -= 0.5f;
	else if (_replaySpeed > 0.25f) _replaySpeed -= 0.25f;
}

void Manager::replayTick() {
	if (_state != AppState::REPLAY || _replayPaused) return;

	static float accumulator = 0.0f;
	accumulator += _replaySpeed;

	while (accumulator >= 1.0f) {
		replayStepForward();
		accumulator -= 1.0f;
	}
}

void Manager::replayStepForward() {
	if (_replayFrame >= static_cast<int>(_history.size())) {
		_state = AppState::FINISHED;
		_sceneData.state = AppState::FINISHED;
		return;
	}
	const auto& snap = _history[_replayFrame];
	_sceneData.cells = snap.cells;
	_sceneData.beam = snap.beam;
	_sceneData.state = AppState::REPLAY;
	_replayFrame++;
}

void Manager::replayStepBack() {
	if (_replayFrame <= 1) return;
	_replayFrame -= 2;   // -2 потому что Forward делает +1
	replayStepForward();
}

void Manager::resetReplay() {
	if (_history.empty()) return;

	_replayFrame = 0;
	_state = AppState::REPLAY;
	_sceneData.state = AppState::REPLAY;

	replayStepForward();
}

void Manager::resetEndReplay() {
	if (_history.empty()) return;

	_replayFrame = static_cast<int>(_history.size()) - 1;
	_state = AppState::REPLAY;
	_sceneData.state = AppState::REPLAY;

	replayStepBack();
}

int Manager::replayCurrentSweep() const {
	int f = _replayFrame - 1;
	if (f >= 0 && f < (int)_history.size())
		return _history[f].sweepNumber;
	return 0;
}