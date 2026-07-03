#pragma once
#include "ISolver.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <array>

namespace Core {
	class MaxElementSolver : public ISolver {
	public:
		MaxElementSolver(int rows, int cols,
			Objects::SensorModel model = {})
			: ISolver(rows, cols, model) {}

		//std::pair<int, int> chooseCell() override;
		//void onSignalResult(int row, int col, double signal) override;
		void buildPlan(int budget) override;
		double calculateGain(int r, int c) const;	
	};
}