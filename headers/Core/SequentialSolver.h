#pragma once
#include "ISolver.h"
#include <vector>
#include <array>

namespace Core {
	class SequentialSolver : public ISolver {
		int _curRow = 0, _curCol = 0;
	public:
		SequentialSolver(int rows, int cols,
			int maxImpulses = 10000,
			Objects::SensorModel model = {})
			: ISolver(rows, cols, maxImpulses, model) {}
		
		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		bool finished() const override;
	};
}