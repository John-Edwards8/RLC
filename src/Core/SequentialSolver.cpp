#include "Core/SequentialSolver.h"

namespace Core {
	std::pair<int, int> SequentialSolver::chooseCell() {
		int startR = _curRow, startC = _curCol;

		for (int attempt = 0; attempt < _rows * _cols; attempt++) {
			if (!_decided[_curRow][_curCol]) return { _curRow, _curCol };

			_curCol++;
			if (_curCol >= _cols) {
				_curCol = 0;
				_curRow++;
				if (_curRow >= _rows) _curRow = 0;
			}

			if (_curRow == startR && _curCol == startC) break;
		}

		return { _curRow, _curCol };
	}

	void SequentialSolver::onSignalResult(int row, int col, double signal) {
		_totalSteps++;
		_n[row][col]++;
		_lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > 0.5) ? 1 : 0;

		// Байесовское обновление
		double b = _belief[row][col];
		double lt = gauss(signal, _model.muTarget, _model.sigmaTarget);
		double ln = gauss(signal, _model.muNoise, _model.sigmaNoise);
		double ev = lt * b + ln * (1.0 - b);
		if (ev > 1e-12)
			_belief[row][col] = (lt * b) / ev;

		std::cout << "Seq Cell (" << row + 1 << "," << col + 1
			<< ") n=" << _n[row][col]
			<< " belief=" << _belief[row][col] << "\n";

		if (_n[row][col] >= 3 && _belief[row][col] < 0.01)
			_decided[row][col] = true;
	}

	bool SequentialSolver::finished() const {
		if (_totalSteps >= _maxImpulses) return true;

		for (int r = 0; r < _rows; r++)
			for (int c = 0; c < _cols; c++)
				if (!_decided[r][c]) return false;

		return true;
	}
}
