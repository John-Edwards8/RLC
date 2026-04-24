#pragma once
#include "ISolver.h"
#include <vector>
#include <array>

namespace Core {
	class SequentialSolver : public ISolver {
		int _rows, _cols;
		int _curRow = 0, _curCol = 0;
		int _totalSteps = 0;
		int _maxImpulses;

		std::vector<std::vector<double>> _belief;
		std::vector<std::vector<int>>    _n;
		std::vector<std::vector<bool>>   _decided;
		std::vector<std::vector<std::array<int, 3>>> _lastBinary;
		Objects::SensorModel _model;
	public:
		SequentialSolver(int rows, int cols,
			int maxImpulses = 10000,
			Objects::SensorModel model = {})
			: _rows(rows), _cols(cols),
			_maxImpulses(maxImpulses), _model(model)
		{
			_belief.assign(rows, std::vector<double>(cols, 1.0 / (rows * cols)));
			_n.assign(rows, std::vector<int>(cols, 0));
			_lastBinary.assign(rows, std::vector<std::array<int, 3>>(cols, {0, 0, 0}));
			_decided.assign(rows, std::vector<bool>(cols, false));
		}
		
		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		double getBelief(int r, int c) override { return _belief[r][c]; }
		int getTotalImpulses() const override	{ return _totalSteps; }
		bool finished() const override;
		int getRecentPositives(int r, int c) const override {
			int measurements = std::min(_n[r][c], 3);
			int count = 0;
			for (int i = 0; i < measurements; i++)
				count += _lastBinary[r][c][i];
			return count;
		}
		void markDecided(int r, int c) override {
			_decided[r][c] = true;
		}
	};
}