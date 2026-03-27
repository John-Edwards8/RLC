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

        // Обновить P(n) = 1 - (1-p)^n
        _detectProb[row][col] = 1.0 - std::pow(1.0 - _p, _n[row][col]);

        double b = _belief[row][col];
        double lt = gauss(signal, _model.muTarget, _model.sigmaTarget);
        double ln = gauss(signal, _model.muNoise, _model.sigmaNoise);
        double ev = lt * b + ln * (1.0 - b);

        if (ev > 1e-12) _belief[row][col] = (lt * b) / ev;

        std::cout << "Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " P=" << _detectProb[row][col]
            << " belief=" << _belief[row][col]
            << " gain=" << calculateGain(row, col) << std::endl;
    }

	bool MaxElementSolver::finished() const {
        if (_totalImpulses >= _maxImpulses) return true;

        for (int r = 0; r < _rows; r++) {
            for (int c = 0; c < _cols; c++) {
                if (_belief[r][c] >= 0.02 && _belief[r][c] < 0.9) return false;
            }
        }
        return true;
    }

	double MaxElementSolver::calculateGain(int r, int c) {
        constexpr int MIN_MEASUREMENTS = 3;
        if (_belief[r][c] >= 0.9)                                 return 0.0;        
        if (_n[r][c] >= MIN_MEASUREMENTS && _belief[r][c] < 0.01) return 0.0;

        // Δ = Q^(n) * p = (1-p)^n * p
        return _belief[r][c] * std::pow(1.0 - _p, _n[r][c]) * _p;
    }
}