#pragma once
#include "ISolver.h"
#include <vector>
#include <array>

namespace Core {
	class SequentialSolver : public ISolver {
		int _curRow = 0, _curCol = 0;
	public:
		SequentialSolver(int rows, int cols,
			Objects::SensorModel model = {})
			: ISolver(rows, cols, model) {}
		
		//std::pair<int, int> chooseCell() override;
		//void onSignalResult(int row, int col, double signal) override;
		void buildPlan(int budget) override;
	};
}