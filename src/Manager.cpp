#include "../headers/Manager.h"

void Manager::initSolver() {
	std::fill(_alreadyDetected.begin(), _alreadyDetected.end(), false);
	_detections.clear();

	switch (_solverType) {
	case Core::SolverType::SEQUENTIAL:
		_solver = std::make_unique<Core::SequentialSolver>(
			_rows, _cols, _impulseLimit, _grid->model);
		break;
	case Core::SolverType::MAX_ELEMENT:
		_solver = std::make_unique<Core::MaxElementSolver>(
			_rows, _cols, _impulseLimit,
			_grid->model.singlePulseDetectProb,
			_grid->model);
		break;
	}
}

Manager::Manager(int impulse, int frequency,
				 int w, int h, int r, int c,
				 Core::SolverType type) :
	_rows(r), _cols(c), _solverType(type), _impulseLimit(impulse), _freq(frequency),
	_display(std::make_unique<Render::Display>(w, h)),
	_grid(std::make_unique<Objects::Grid>(r, c)),
	_alreadyDetected(r* c, false)
{
	int maxDim = std::max(r, c);
	_cellSize = (h * 9 / 10) / maxDim;
	_borderX = (w - _cellSize * c) / 2;
	_borderY = (h - _cellSize * r) / 2;

	_sceneData.rows = r;
	_sceneData.cols = c;
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
	if (_state != AppState::RUNNING) return;
	auto [row, col] = _solver->chooseCell();
	auto& cell = _grid->coords[row][col];

	if (_solver->getBelief(row, col) >= 0.9) {
		_solver->onSignalResult(row, col, 0.0);
		std::cout << "Already confident about cell (" << row + 1 << "," << col + 1 << "), skipping measurement." << std::endl;
		return;
	}

	_sceneData.beam.x = _borderX + col * _cellSize + _cellSize / 2.0f;
	_sceneData.beam.y = _borderY + row * _cellSize + _cellSize / 2.0f;

	double signal = _grid->measure(row, col);

	_solver->onSignalResult(row, col, signal);

	cell.impulsesSent++;
	cell.detectProb = _grid->computeDetectionProb(cell.impulsesSent);
	
	if (_solver->getBelief(row, col) >= 0.9 && !_alreadyDetected[row * _cols + col]) {
		_alreadyDetected[row * _cols + col] = true;
		_detections.push_back({ row, col, _solver->getTotalImpulses(), _grid->coords[row][col].realTarget });
		std::cout << "TARGET DETECTED at (" << row + 1 << "," << col + 1
			<< ") after " << _solver->getTotalImpulses() << " impulses\n";
	}

	updateScene();

	render();

	if (_solver->finished()) {
		_state = AppState::FINISHED;
		_sceneData.state = AppState::FINISHED;
		std::cout << "\n=== SEARCH COMPLETE ===" << std::endl;

		int found = 0;
		for (auto& d : _detections) {
			bool correct = _grid->coords[d.row][d.col].realTarget;
			std::cout << "  (" << d.row + 1 << "," << d.col + 1 << ") "
				<< (correct ? "CORRECT" : "FALSE POSITIVE")
				<< " - impulses: " << d.impulsesAtDetection << "\n";
			if (correct) found++;
		}
		std::cout << "Total found: " << found << " of " << _grid->getTargetCount() << std::endl;

		if (!_detections.empty()) {
			double tMin = std::numeric_limits<double>::max();
			double tMax = 0.0;
			double tSum = 0.0;
			for (auto& d : _detections) {
				double t = static_cast<double>(d.impulsesAtDetection) / _freq;
				tMin = std::min(tMin, t);
				tMax = std::max(tMax, t);
				tSum += t;
			}
			double tAvg = tSum / _detections.size();
			std::cout << "T_detect (sec): min=" << tMin
				<< " avg=" << tAvg
				<< " max=" << tMax
				<< " (freq=" << _freq << " Hz)\n";
		}
		return;
	}
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
}

/*
* TODO: Move logging to a separate class and make it more structured (e.g. JSON or CSV) for easier analysis.
void Manager::log(int cI, int cJ, int imp, int freq, int impCnt, int dImpCnt){
	auto c = (*(coords + cJ) + cI);
	ofstream file("logs.txt", ios::app);
	if(first && !logging) {
		time_t tt;
	    struct tm* ti;
	    time(&tt);
	    ti = localtime(&tt);
	  
		file << endl << asctime(ti) << endl;
		logging = true;
	}
	if(!first) { file << "У клітинку (" << cI+1 << "," << cJ+1 << ") направлено " << (!c->target? impCnt : dImpCnt) << " імпульсів." << endl; }
	else { file << "У клітинку (" << cI+1 << "," << cJ+1 << ") направлено " << impCnt << " імпульсів." << endl; }

	if (c->target) { file << "Отримано позитивну відповідь." << endl; }

	if(c->targetChecker >= 0.9 && c->isFound == false) {
		file << "У строці " << cI+1 << "," << " та колонці " << cJ+1 << ", знайдено ціль за " << (double)imp/freq << " секунд." << endl;
	}
	file.close();
}
*/