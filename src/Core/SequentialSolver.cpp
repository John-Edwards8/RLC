#include "Core/SequentialSolver.h"

namespace Core {
	/*
	std::pair<int, int> SequentialSolver::chooseCell() {
		auto cell = std::make_pair(_curRow, _curCol);
		if (++_curCol >= _cols) {
			_curCol = 0;
			if (++_curRow >= _rows) _curRow = 0;
		}
		return cell;
	}

	void SequentialSolver::onSignalResult(int row, int col, double signal) {
		_totalImpulses++;
		_sweepImpulses++;
		_n[row][col]++;
		_lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;

		// Байесовское обновление
		bayesUpdate(row, col, signal);

		std::cout << "Seq Cell (" << row + 1 << "," << col + 1
			<< ") n=" << _n[row][col]
			<< " belief=" << _belief[row][col] << "\n";
	}
	*/

	void SequentialSolver::buildPlan(int budget) {
		_sweepPlan.clear();
		int impulsesPerCell = budget / (_rows * _cols);
		for (int r = 0; r < _rows; r++)
			for (int c = 0; c < _cols; c++)
				for (int t = 0; t < impulsesPerCell; t++)
					_sweepPlan.push_back({ r, c });
	}
}
