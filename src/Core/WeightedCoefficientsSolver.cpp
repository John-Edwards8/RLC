#include "Core/WeightedCoefficientsSolver.h"

namespace Core {
    double WeightedCoefficientsSolver::calculateWeight(int r, int c) const {
        //constexpr int MIN_MEASUREMENTS = 3;
        //if (_belief[r][c] >= 0.9)                                 return 0.0;
        //if (_n[r][c] >= MIN_MEASUREMENTS && _belief[r][c] < 0.01) return 0.0;

        double eps = 1.0 - effectiveDetectProb();

        if (_n[r][c] == 0) {
            // lim_{n->0} (1 - eps^n) / n = -ln(eps)
            return _belief[r][c] * (-std::log(eps));
        }

        return _belief[r][c] * (1.0 - std::pow(eps, _n[r][c])) / _n[r][c];
    }
    /*
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
        _sweepImpulses++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;
        bayesUpdate(row, col, signal);

        std::cout << "WC Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " belief=" << _belief[row][col]
            << " weight=" << calculateWeight(row, col) << "\n";
    }
    */

    // Метод весовых коэффициентов по Берзину (алг. 3.1).
    void WeightedCoefficientsSolver::buildPlan(int budget) {
        _sweepPlan.clear();

        double p = effectiveDetectProb();
        double eps = 1.0 - p;

        // Вес ячейки ω_l = (1 - eps^n) / n — МВК формула
        // При n=0: lim = -ln(eps)
        std::vector<std::vector<double>> delta(_rows, std::vector<double>(_cols, 0.0));
        std::vector<std::vector<int>>    nVirt(_rows, std::vector<int>(_cols, 0));
        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++) {
                nVirt[r][c] = _n[r][c];
                //if (_decided[r][c]) { delta[r][c] = 0.0; continue; }
                int    n = _n[r][c];
                double omega = (n == 0)
                    ? (-std::log(eps))                              // lim при n->0
                    : (1.0 - std::pow(eps, n)) / n;
                delta[r][c] = _belief[r][c] * omega;
            }

        for (int t = 0; t < budget; t++) {
            // 3°: argmax Δ (формула 3.25)
            int bestR = -1, bestC = -1;
            double maxD = 0.0;
            for (int r = 0; r < _rows; r++)
                for (int c = 0; c < _cols; c++)
                    if (delta[r][c] > maxD) {
                        maxD = delta[r][c];
                        bestR = r; bestC = c;
                    }

            if (bestR == -1) break;
            _sweepPlan.push_back({ bestR, bestC });

            // 4°: пересчёт delta для выбранной ячейки
            // n виртуально увеличивается на 1
            nVirt[bestR][bestC]++;
            int    n_new = nVirt[bestR][bestC];
            double omega_new = (1.0 - std::pow(eps, n_new)) / n_new;
            delta[bestR][bestC] = _belief[bestR][bestC] * omega_new;
        }
    }
}