#include "../headers/Manager.h"

Manager::Manager(int impulse, int frequency,
				 int w, int h, int r, int c,
				 Core::SolverType type) :
	_rows(r), _cols(c), _solverType(type),
	_display(std::make_unique<Render::Display>(w, h)),
	_grid(std::make_unique<Objects::Grid>(r, c))
{
	switch (type) {
	case Core::SolverType::SEQUENTIAL:
		_solver = std::make_unique<Core::SequentialSolver>(r, c, impulse, _grid->model);
		break;
	case Core::SolverType::MAX_ELEMENT:
		_solver = std::make_unique<Core::MaxElementSolver>(
			r, c, impulse,
			_grid->model.singlePulseDetectProb,
			_grid->model
		);
		break;
	}

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
	_sceneData.beamActive = false;

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
			cellInfo.confidence = gridCell.belief;

			_sceneData.cells.push_back(cellInfo);
		}
	}
}

// Байесовское обновление
void Manager::updateBelief(Objects::Cell& cell, double signal) {
	double priorTarget = (cell.belief > 0.0) ? cell.belief : 0.1;
	double priorNoTarget = 1.0 - priorTarget;

	auto gauss = [](double x, double mu, double sig) {
		return (1.0 / (sig * 2.5066282746)) * std::exp(-0.5 * ((x - mu) / sig) * ((x - mu) / sig));
	};

	double likeTarget   = gauss(signal, _grid->model.muTarget, _grid->model.sigmaTarget);
	double likeNoTarget = gauss(signal, _grid->model.muNoise, _grid->model.sigmaNoise);

	double evidence = likeTarget * priorTarget + likeNoTarget * priorNoTarget;
	if (evidence < 1e-12) return;

	cell.belief = (likeTarget * priorTarget) / evidence;
}

void Manager::step() {
	_sceneData.beamActive = true;
	auto [row, col] = _solver->chooseCell();
	auto& cell = _grid->coords[row][col];

	if (cell.belief >= 0.9) {
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
	
	updateBelief(cell, signal);

	updateScene();

	render();

	if (_solver->finished()) {
		_sceneData.beamActive = false;
		std::cout << "\n=== SEARCH COMPLETE ===" << std::endl;

		int foundTargets = 0;
		for (int r = 0; r < _rows; ++r) {
			for (int c = 0; c < _cols; ++c) {
				if (_grid->coords[r][c].belief >= 0.9) {
					foundTargets++;
					bool correct = _grid->coords[r][c].realTarget;
					std::cout << "Found target at (" << r + 1 << "," << c + 1 << ") - "
						<< (correct ? "CORRECT" : "FALSE POSITIVE") << std::endl;
				}
			}
		}
		std::cout << "Total found: " << foundTargets << " of " << _grid->getTargetCount() << std::endl;

		return;
	}
}

void Manager::render() {
	_display->getRenderer()->clear();
	_display->getRenderer()->render(_sceneData);
	_display->getRenderer()->present();
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