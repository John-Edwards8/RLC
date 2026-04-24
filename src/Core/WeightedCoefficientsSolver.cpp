#include "Core/WeightedCoefficientsSolver.h"

namespace Core {
    double WeightedCoefficientsSolver::calculateWeight(int r, int c) const {
        constexpr int MIN_MEASUREMENTS = 3;
        if (_belief[r][c] >= 0.9)                                  return 0.0;
        if (_n[r][c] >= MIN_MEASUREMENTS && _belief[r][c] < 0.01) return 0.0;

        double eps = 1.0 - effectiveDetectProb();

        if (_n[r][c] == 0) {
            // lim_{n->0} (1 - eps^n) / n = -ln(eps)
            return _belief[r][c] * (-std::log(eps));
        }

        return _belief[r][c] * (1.0 - std::pow(eps, _n[r][c])) / _n[r][c];
    }

    std::pair<int, int> WeightedCoefficientsSolver::chooseCell() {
        double bestWeight = -std::numeric_limits<double>::max();
        int bestR = 0, bestC = 0;

        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++) {
                double w = calculateWeight(r, c);
                if (w > bestWeight) {
                    bestWeight = w;
                    bestR = r;
                    bestC = c;
                }
            }

        return { bestR, bestC };
    }

    void WeightedCoefficientsSolver::onSignalResult(int row, int col, double signal) {
        _n[row][col]++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;
        bayesUpdate(row, col, signal);

        std::cout << "WC Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " belief=" << _belief[row][col]
            << " weight=" << calculateWeight(row, col) << "\n";
    }

    bool WeightedCoefficientsSolver::finished() const {
        if (_totalImpulses >= _maxImpulses) return true;

        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++)
                if (_belief[r][c] >= 0.02 && _belief[r][c] < 0.9)
                    return false;

        return true;
    }
}