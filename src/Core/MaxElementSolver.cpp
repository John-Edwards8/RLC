#include "Core/MaxElementSolver.h"

namespace Core {
    /*
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
        _sweepImpulses++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;

        bayesUpdate(row, col, signal);

        std::cout << "Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " belief=" << _belief[row][col]
            << " gain=" << calculateGain(row, col) << std::endl;
    }
    */
    // Метод максимального элемента по Берзину(алг. 1.1).
    //
    // budget = rows*cols "посещений". Каждое посещение = burstSize импульсов.
    // Адаптивность: алгоритм сам решает сколько раз поставить ячейку в план —
    // высококонфидентные ячейки получают больше посещений, т.к. их delta выше.
    //
    // Формулы:
    //   delta[r][c] = belief[r][c] * p * eps^n   (1.12)
    //   argmax delta                             (1.13)
    //   delta[best] *= eps                       (1.16)
    void MaxElementSolver::buildPlan(int budget) {
        _sweepPlan.clear();

        double p = effectiveDetectProb();
        double eps = 1.0 - p;

        // Виртуальный вектор Δ (формула 1.12)
        std::vector<std::vector<double>> delta(_rows, std::vector<double>(_cols));
        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++)
                //delta[r][c] = _decided[r][c] ? 0.0
                //: _belief[r][c] * p * std::pow(eps, _n[r][c]);
                delta[r][c] = _belief[r][c] * p * std::pow(eps, _n[r][c]);

        for (int t = 0; t < budget; t++) {
            // Формула 1.13 — argmax Δ
            int bestR = -1, bestC = -1;
            double maxD = 0.0;
            for (int r = 0; r < _rows; r++)
                for (int c = 0; c < _cols; c++)
                    if (delta[r][c] > maxD) {
                        maxD = delta[r][c];
                        bestR = r; bestC = c;
                    }

            if (bestR == -1) break;  // все delta == 0, план исчерпан

            _sweepPlan.push_back({ bestR, bestC });
            delta[bestR][bestC] *= eps;  // формула 1.16
        }
    }

	double MaxElementSolver::calculateGain(int r, int c) const {
        //if (_decided[r][c]) return 0.0;
        //if (_n[r][c] >= 3 && _belief[r][c] < 0.01) return 0.0;

        // Δ = Q^(n) * p = (1-p)^n * p
        double p = effectiveDetectProb();
        return _belief[r][c] * std::pow(1.0 - p, _n[r][c]) * p;
    }
}