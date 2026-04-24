#include "Core/MaxElementSolver.h"

namespace Core {
	std::pair<int, int> MaxElementSolver::chooseCell() {
        double bestGain = -1.0;
        int bestR = 0, bestC = 0;

        for (int r = 0; r < _rows; r++) {
            for (int c = 0; c < _cols; c++) {
                double gain = calculateGain(r, c);
                if (gain > bestGain) {
                    bestGain = gain;
                    bestR = r;
                    bestC = c;
                }
            }
        }

        return { bestR, bestC };
    }

	void MaxElementSolver::onSignalResult(int row, int col, double signal) {
        _n[row][col]++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > 0.5) ? 1 : 0;

        // Обновить P(n) = 1 - (1-p)^n
        //_detectProb[row][col] = 1.0 - std::pow(1.0 - _p, _n[row][col]);

        bayesUpdate(row, col, signal);

        std::cout << "Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            //<< " P=" << _detectProb[row][col]
            << " belief=" << _belief[row][col]
            << " gain=" << calculateGain(row, col) << std::endl;

        if (_n[row][col] >= 3 && _belief[row][col] < 0.01)
            _decided[row][col] = true;
    }

	bool MaxElementSolver::finished() const {
        if (_totalImpulses >= _maxImpulses) return true;

        for (int r = 0; r < _rows; r++) {
            for (int c = 0; c < _cols; c++) {
                if (calculateGain(r, c) > 0.0) return false;
            }
        }
        return true;
    }

	double MaxElementSolver::calculateGain(int r, int c) const {
        constexpr int MIN_MEASUREMENTS = 3;
        if (_decided[r][c])  return 0.0;
        if (_n[r][c] >= MIN_MEASUREMENTS && _belief[r][c] < 0.01) return 0.0;

        // Δ = Q^(n) * p = (1-p)^n * p
        return _belief[r][c] * std::pow(1.0 - _p, _n[r][c]) * _p;
    }
}