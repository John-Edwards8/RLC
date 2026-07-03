#pragma once
#include "Objects.h"
#include <utility>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>

namespace Core {
    enum class SolverType {
        SEQUENTIAL,
        MAX_ELEMENT,
        TWO_FUNCTIONS,
        WEIGHTED_COEFFICIENTS
    };

    // Снимок плана buildPlan() — для визуализации распределения ресурса
    struct PlanSnapshot {
        std::vector<std::vector<int>> allocations; // сколько импульсов назначено ячейке в этом прогоне
        std::vector<std::vector<int>> visitOrder;  // номер шага первого посещения (0 = не посещается)
        std::vector<std::vector<int>> visitRank;
        int sweepNumber = 0;                       // номер прогона (1, 2, 3)
    };

    class ISolver {
        std::vector<std::tuple<int, int, double>> _lastSweepSignals;
        PlanSnapshot _lastPlan;
        std::array<PlanSnapshot, 5> _allPlans;
    protected:
        int _rows, _cols;
        int _totalImpulses = 0;
        int _sweepCount = 0; // сколько прогонов уже завершено
        std::vector<std::vector<double>> _belief;
        std::vector<std::vector<bool>>   _decided;
        std::vector<std::vector<int>>    _n;
        std::vector<std::vector<std::array<int, 3>>> _lastBinary;
        Objects::SensorModel _model;

        ISolver(int rows, int cols, Objects::SensorModel model)
            : _rows(rows), _cols(cols), _model(model)
        {
            _belief.assign(rows, std::vector<double>(cols, 1.0 / (rows * cols)));
            _decided.assign(rows, std::vector<bool>(cols, false));
            _n.assign(rows, std::vector<int>(cols, 0));
            _lastBinary.assign(rows, std::vector<std::array<int, 3>>(cols, { 0,0,0 }));
        }

        void bayesUpdate(int row, int col, double signal) {
            double b = _belief[row][col];
            double lt = gauss(signal, _model.muTarget, _model.sigmaTarget);
            double ln = gauss(signal, _model.muNoise, _model.sigmaNoise);
            double ev = lt * b + ln * (1.0 - b);
            if (ev > 1e-12) _belief[row][col] = (lt * b) / ev;
        }

        std::vector<std::pair<int, int>> _sweepPlan;   // текущий план прогона
        int _sweepIndex = 0;                           // где мы в плане
        std::vector<std::tuple<int, int, double>> _pendingSignals; // накопленные сигналы

        virtual void buildPlan(int budget) = 0;  // каждый солвер строит свой план

        bool sweepDone() const { return _sweepIndex >= (int)_sweepPlan.size(); }
        void capturePlanSnapshot() {
            _lastPlan.allocations.assign(_rows, std::vector<int>(_cols, 0));
            _lastPlan.visitOrder.assign(_rows, std::vector<int>(_cols, 0));
            _lastPlan.visitRank.assign(_rows, std::vector<int>(_cols, 0));
            _lastPlan.sweepNumber = _sweepCount;

            int step = 1;
            for (auto& [r, c] : _sweepPlan) {
                _lastPlan.allocations[r][c] += 1;
                if (_lastPlan.visitOrder[r][c] == 0)
                    _lastPlan.visitOrder[r][c] = step;
                ++step;
            }

            int rank = 1;
            for (auto& [r, c] : _sweepPlan) {
                if (_lastPlan.visitRank[r][c] == 0) {
                    _lastPlan.visitRank[r][c] = rank++;
                }
            }
        }
    public:
        const std::vector<std::tuple<int, int, double>>& getLastSweepSignals() const {
            return _lastSweepSignals;
        }
        const PlanSnapshot& getLastPlan() const { return _lastPlan; }
        const PlanSnapshot& getPlanSnapshot(int sweepIndex) const {
            static PlanSnapshot empty{};
            if (sweepIndex >= 0)
                return _allPlans[sweepIndex];
            return empty;
        }
        bool isSweepDone() const {
            return _sweepIndex >= (int)_sweepPlan.size(); }
        int getN(int r, int c) const { return _n[r][c]; }

        void flushPending() {
            _lastSweepSignals = _pendingSignals;  // сохранить для Manager
            for (auto& [r, c, s] : _pendingSignals)
                bayesUpdate(r, c, s);
            _pendingSignals.clear();
        }
        std::pair<int, int> chooseCell(int budget) {
            if (sweepDone()) {
                if (_sweepCount > 0) flushPending();
                buildPlan(budget);      // построить новый план
                _sweepCount++;
                capturePlanSnapshot();
                int si = _sweepCount - 1;
                if (si >= 0) _allPlans[si] = _lastPlan;
                _sweepIndex = 0;
            }
            return _sweepPlan[_sweepIndex++];
        }

        void onSignalResult(int row, int col, double signal) {
            _n[row][col]++;
            _totalImpulses++;
            _lastBinary[row][col][(_n[row][col] - 1) % 3] = (signal > optimalThreshold()) ? 1 : 0;
            _pendingSignals.emplace_back(row, col, signal);  // НЕ обновляем belief сразу
        }

        bool finished() const {
            return _sweepCount >= (int)_allPlans.size();
        }
        virtual ~ISolver() = default;
        void markDecided(int r, int c) { _decided[r][c] = true; }
        double getBelief(int r, int c) { return _belief[r][c]; }
        int getTotalImpulses() const { return _totalImpulses; }
        int getSweepCount()    const { return _sweepCount; }

        int getRecentPositives(int r, int c) const {
            int measurements = std::min(_n[r][c], 3);
            int count = 0;
            for (int i = 0; i < measurements; i++)
                count += _lastBinary[r][c][i];
            return count;
        }

/*        double effectiveDetectProb() const {
            // P(signal > 0.5 | target) по нормальному распределению
            // = P(Z > (0.5 - mu) / sigma) = 1 - Φ((0.5 - muTarget) / sigmaTarget)
            double z = (optimalThreshold() - _model.muTarget) / _model.sigmaTarget;
            return 0.5 * std::erfc(z / std::sqrt(2.0));
        }

        double optimalThreshold() const {
            // При равных sigma — среднее между mu
            return (_model.muTarget + _model.muNoise) / 2.0;
        }*/

        double optimalThreshold() const {
            // Для Pfa = 1e-6, коэффициент Z (квантиль) примерно равен 4.75
            // В реальности вычисляется как: sqrt(2) * erfc_inv(2 * targetPfa)

            double z = 4.7534; // Значение для Pfa = 10^-6
            return _model.muNoise + z * _model.sigmaNoise;
        }

        double effectiveDetectProb() const {
            double threshold = optimalThreshold();
            // Теперь смотрим, какая часть сигнала muTarget окажется выше этого жесткого порога
            double z = (threshold - _model.muTarget) / _model.sigmaTarget;
            return 0.5 * std::erfc(z / std::sqrt(2.0));
        }

        static double gauss(double x, double mu, double sig) {
            static constexpr double SQRT_2PI = 2.5066282746;
            return (1.0 / (sig * SQRT_2PI)) *
                std::exp(-0.5 * ((x - mu) / sig) * ((x - mu) / sig));
        }

        void normalizeBeliefs() {
            double sum = 0.0;
            for (int r = 0; r < _rows; r++)
                for (int c = 0; c < _cols; c++)
                    sum += _belief[r][c];
            if (sum > 1e-12)
                for (int r = 0; r < _rows; r++)
                    for (int c = 0; c < _cols; c++)
                        _belief[r][c] /= sum;
        }
    };
}

/*#pragma once
#include "Objects.h"
#include <utility>
#include <vector>
#include <array>
#include <cmath>
#include <iostream>

namespace Core {
	enum class SolverType {
		SEQUENTIAL,
		MAX_ELEMENT,
		TWO_FUNCTIONS,
		WEIGHTED_COEFFICIENTS
	};

	class ISolver {
	protected:
		int _rows, _cols;
		int _totalImpulses = 0;
		int _sweepImpulses = 0;
		std::vector<std::vector<double>> _belief;
		std::vector<std::vector<bool>>   _decided;
		std::vector<std::vector<int>>    _n;
		std::vector<std::vector<std::array<int, 3>>> _lastBinary;
		Objects::SensorModel _model;
		
		ISolver(int rows, int cols, Objects::SensorModel model)
			: _rows(rows), _cols(cols), _model(model)
		{
			_belief.assign(rows, std::vector<double>(cols, 1.0 / (rows * cols)));
			_decided.assign(rows, std::vector<bool>(cols, false));
			_n.assign(rows, std::vector<int>(cols, 0));
			_lastBinary.assign(rows, std::vector<std::array<int, 3>>(cols, { 0,0,0 }));
		}

		void bayesUpdate(int row, int col, double signal) {
			double b = _belief[row][col];
			double lt = gauss(signal, _model.muTarget, _model.sigmaTarget);
			double ln = gauss(signal, _model.muNoise, _model.sigmaNoise);
			double ev = lt * b + ln * (1.0 - b);
			if (ev > 1e-12) _belief[row][col] = (lt * b) / ev;
		}
	public:
		virtual std::pair<int, int> chooseCell() = 0;
		virtual void onSignalResult(int row, int col, double signal) = 0;

		void markDecided(int r, int c) { _decided[r][c] = true; }
		double getBelief(int r, int c) { return _belief[r][c]; }
		int getTotalImpulses() const { return _totalImpulses; }
		int getN(int r, int c) const { return _n[r][c]; }

		int getRecentPositives(int r, int c) const {
			int measurements = std::min(_n[r][c], 3);
			int count = 0;
			for (int i = 0; i < measurements; i++)
				count += _lastBinary[r][c][i];
			return count;
		}

		double effectiveDetectProb() const {
			// P(signal > 0.5 | target) по нормальному распределению
			// = P(Z > (0.5 - mu) / sigma) = 1 - Φ((0.5 - muTarget) / sigmaTarget)
			double z = (0.5 - _model.muTarget) / _model.sigmaTarget;
			return 0.5 * std::erfc(z / std::sqrt(2.0));
		}

		double optimalThreshold() const {
			// При равных sigma — среднее между mu
			return (_model.muTarget + _model.muNoise) / 2.0;
		}

		static double gauss(double x, double mu, double sig) {
			static constexpr double SQRT_2PI = 2.5066282746;
			return (1.0 / (sig * SQRT_2PI)) *
				std::exp(-0.5 * ((x - mu) / sig) * ((x - mu) / sig));
		}

		int getSweepImpulses() const { return _sweepImpulses; }
		void resetSweepImpulses() { _sweepImpulses = 0; }

		void normalizeBeliefs() {
			double sum = 0.0;
			for (int r = 0; r < _rows; r++)
				for (int c = 0; c < _cols; c++)
					sum += _belief[r][c];
			if (sum > 1e-12)
				for (int r = 0; r < _rows; r++)
					for (int c = 0; c < _cols; c++)
						_belief[r][c] /= sum;
		}
	};
}
*/