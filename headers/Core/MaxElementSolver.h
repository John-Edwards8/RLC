#pragma once
#include "ISolver.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace Core {
	class MaxElementSolver : public ISolver {
		int _rows, _cols;
		std::vector<std::vector<int>> _n; // n[r][c]
		std::vector<std::vector<double>> _detectProb; // P_i(n)
		std::vector<std::vector<double>> _belief; // P(цель | сигналы)
		
		Objects::SensorModel _model;
		
		double _p; // p
		int _totalImpulses = 0;
		int _maxImpulses = 1000;
	public:
		MaxElementSolver(int rows, int cols,
			int maxImpulses = 1000,
			double singlePulseProb = 0.5,
			Objects::SensorModel model = {})
			: _rows(rows), _cols(cols),
			_maxImpulses(maxImpulses), _p(singlePulseProb), _model(model)
		{
			_n.assign(rows, std::vector<int>(cols, 0));
			_detectProb.assign(rows, std::vector<double>(cols, 0.0));
			_belief.assign(rows, std::vector<double>(cols, 0.1));
		}

		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		bool finished() const override;

		double calculateGain(int r, int c);

		// Heatmap visualization support
		double getDetectProb(int r, int c) const { return _detectProb[r][c]; }
		double getGain(int r, int c)			 { return calculateGain(r, c); }
		double getBelief(int r, int c)			 { return _belief[r][c]; }
	};
}