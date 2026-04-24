#pragma once
#include "ISolver.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <array>

namespace Core {
	class MaxElementSolver : public ISolver {
		double _p; // p
	public:
		MaxElementSolver(int rows, int cols,
			int maxImpulses = 10000,
			double singlePulseProb = 0.5,
			Objects::SensorModel model = {})
			: ISolver(rows, cols, maxImpulses, model), _p(singlePulseProb)
		{
			//_detectProb.assign(rows, std::vector<double>(cols, 0.0));
		}

		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		bool finished() const override;
		
		double calculateGain(int r, int c) const;

		// Heatmap visualization support ?
		//double getDetectProb(int r, int c) const { return _detectProb[r][c]; }		
	};
}