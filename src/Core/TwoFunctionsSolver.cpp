#include "Core/TwoFunctionsSolver.h"

namespace Core {
    double TwoFunctionsSolver::calculateGain(int r, int c) const {
        constexpr int MIN_MEASUREMENTS = 3;
        if (_belief[r][c] >= 0.9)                                 return 0.0;
        if (_n[r][c] >= MIN_MEASUREMENTS && _belief[r][c] < 0.01) return 0.0;

        double eps = 1.0 - effectiveDetectProb();

        // F+: прирост выигрыша от сканирования ячейки (r,c)
        double epsPow = (eps > 1e-12) ? std::pow(eps, _n[r][c] - 1) : 0.0;
        double gainPlus = _belief[r][c] * (1.0 + epsPow);

        // F-: упущенная выгода по остальным ячейкам (формула 2.17 Берзина)
        double lossSum = 0.0;
        for (int i = 0; i < _rows; i++)
            for (int j = 0; j < _cols; j++) {
                if (i == r && j == c) continue;
                lossSum += _belief[i][j] * std::pow(eps, _n[i][j]);
            }
        double lossTerm = (eps > 1e-12) ? lossSum / eps : 0.0;

        return gainPlus - lossTerm;
    }

    std::pair<int, int> TwoFunctionsSolver::chooseCell() {
        double bestGain = -std::numeric_limits<double>::max();
        int bestR = 0, bestC = 0;

        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++) {
                if (_decided[r][c]) continue;
                double gain = calculateGain(r, c);
                if (gain > bestGain) {
                    bestGain = gain;
                    bestR = r;
                    bestC = c;
                }
            }

        return { bestR, bestC };
    }

    void TwoFunctionsSolver::onSignalResult(int row, int col, double signal) {
        _n[row][col]++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;
        bayesUpdate(row, col, signal);

        std::cout << "TF Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " belief=" << _belief[row][col]
            << " gain=" << calculateGain(row, col) << "\n";
    }

    bool TwoFunctionsSolver::finished() const {
        if (_totalImpulses >= _maxImpulses) return true;

        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++) {
                if (_decided[r][c]) continue;
                if (calculateGain(r, c) != 0.0) return false;
            }

        return true;
    }
}