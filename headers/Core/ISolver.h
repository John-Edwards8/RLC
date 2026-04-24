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
		MAX_ELEMENT
	};

	class ISolver {
	protected:
		int _rows, _cols;
		int _totalImpulses = 0;
		int _maxImpulses;
		std::vector<std::vector<double>> _belief;
		std::vector<std::vector<bool>>   _decided;
		std::vector<std::vector<int>>    _n;
		std::vector<std::vector<std::array<int, 3>>> _lastBinary;
		Objects::SensorModel _model;
		
		ISolver(int rows, int cols, int maxImpulses, Objects::SensorModel model)
			: _rows(rows), _cols(cols), _maxImpulses(maxImpulses), _model(model)
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
		virtual bool finished() const = 0;

		void markDecided(int r, int c) { _decided[r][c] = true; }
		double getBelief(int r, int c) { return _belief[r][c]; }
		int getTotalImpulses() const { return _totalImpulses; }

		int getRecentPositives(int r, int c) const {
			int measurements = std::min(_n[r][c], 3);
			int count = 0;
			for (int i = 0; i < measurements; i++)
				count += _lastBinary[r][c][i];
			return count;
		}

		static double gauss(double x, double mu, double sig) {
			static constexpr double SQRT_2PI = 2.5066282746;
			return (1.0 / (sig * SQRT_2PI)) *
				std::exp(-0.5 * ((x - mu) / sig) * ((x - mu) / sig));
		}
	};
}