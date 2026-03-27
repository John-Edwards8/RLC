#pragma once
#include "ISolver.h"

namespace Core {
	class SequentialSolver : public ISolver {
		int _rows, _cols;
		int _curRow = 0, _curCol = 0;
		int _totalSteps = 0;
		int _maxImpulses;

		std::vector<std::vector<double>> _belief;
		std::vector<std::vector<int>>    _n;

		Objects::SensorModel _model;
	public:
		SequentialSolver(int rows, int cols,
			int maxImpulses = 10000,
			Objects::SensorModel model = {})
			: _rows(rows), _cols(cols),
			_maxImpulses(maxImpulses), _model(model)
		{
			_belief.assign(rows, std::vector<double>(cols, 0.1));
			_n.assign(rows, std::vector<int>(cols, 0));
		}
		
		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		bool finished() const override;

		bool cellDecided(int r, int c) const {
			constexpr int MIN_MEAS = 3;
			if (_belief[r][c] >= 0.9)                         return true; // найдена
			if (_n[r][c] >= MIN_MEAS && _belief[r][c] < 0.01) return true; // исключена
			return false;
		}
	};
}