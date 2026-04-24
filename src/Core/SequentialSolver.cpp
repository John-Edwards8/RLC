#include "Core/SequentialSolver.h"

namespace Core {
	std::pair<int, int> SequentialSolver::chooseCell() {
		for (int attempt = 0; attempt < _rows * _cols; attempt++) {
			int r = _curRow, c = _curCol;

			if (++_curCol >= _cols) {
				_curCol = 0;
				if (++_curRow >= _rows) _curRow = 0;
			}

			if (!_decided[r][c]) return { r, c };
		}
		return { _curRow, _curCol };
	}

	void SequentialSolver::onSignalResult(int row, int col, double signal) {
		_totalImpulses++;
		_n[row][col]++;
		_lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;

		// Байесовское обновление
		bayesUpdate(row, col, signal);

		std::cout << "Seq Cell (" << row + 1 << "," << col + 1
			<< ") n=" << _n[row][col]
			<< " belief=" << _belief[row][col] << "\n";

		if (_n[row][col] >= 3 && _belief[row][col] < 0.01)
			_decided[row][col] = true;
	}

	bool SequentialSolver::finished() const {
		if (_totalImpulses >= _maxImpulses) return true;

		for (int r = 0; r < _rows; r++)
			for (int c = 0; c < _cols; c++)
				if (!_decided[r][c]) return false;

		return true;
	}
}
