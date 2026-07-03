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
    /*
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
        _sweepImpulses++;
        _totalImpulses++;
        _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;
        bayesUpdate(row, col, signal);

        std::cout << "TF Cell (" << row + 1 << "," << col + 1
            << ") n=" << _n[row][col]
            << " belief=" << _belief[row][col]
            << " gain=" << calculateGain(row, col) << "\n";
    }
    */

    // Метод двух функционалов по Берзину (алг. 2.1).
    void TwoFunctionsSolver::buildPlan(int budget) {
        _sweepPlan.clear();

        double p = effectiveDetectProb();
        double eps = 1.0 - p;

        // A_l^(0) = belief, a_l^(0) = eps^n (формула 2.21')
        std::vector<std::vector<double>> A(_rows, std::vector<double>(_cols));
        std::vector<std::vector<double>> a(_rows, std::vector<double>(_cols));
        for (int r = 0; r < _rows; r++)
            for (int c = 0; c < _cols; c++) {
                A[r][c] = /*_decided[r][c] ? 0.0 :*/ _belief[r][c];
                a[r][c] = std::pow(eps, _n[r][c]);
            }

        for (int t = 0; t < budget; t++) {
            // 1°: вычислить Δ_kl = A_l * ω_hl - сумма (формула 2.21)
            // Для одного средства: Δ_l = A_l * p * a_l - loss
            // F+ компонента для каждой ячейки:
            double totalLoss = 0.0;
            for (int r = 0; r < _rows; r++)
                for (int c = 0; c < _cols; c++)
                    totalLoss += A[r][c] * a[r][c];

            // 2°: argmax Δ (формула 2.22)
            int bestR = -1, bestC = -1;
            double maxD = -std::numeric_limits<double>::max();

            for (int r = 0; r < _rows; r++) {
                for (int c = 0; c < _cols; c++) {
                    if (A[r][c] <= 0.0) continue;
                    // F+: прирост от зондирования (r,c)
                    // F-: потеря от незондирования остальных
                    double gainPlus = A[r][c] * (1.0 - eps) * a[r][c];
                    double lossTerm = (totalLoss - A[r][c] * a[r][c]) * (1.0 - eps);
                    double delta = gainPlus - lossTerm;
                    if (delta > maxD) {
                        maxD = delta;
                        bestR = r; bestC = c;
                    }
                }
            }

            if (bestR == -1) break;
            _sweepPlan.push_back({ bestR, bestC });

            // 4°: пересчёт A и a (формула 2.24)
            // A_l^(t) = A_l^(t-1) * eps для выбранной ячейки
            A[bestR][bestC] *= eps;
            a[bestR][bestC] *= eps;
        }
    }
}