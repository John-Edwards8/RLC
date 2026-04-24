#pragma once
#include <random>
#include <iostream>

namespace Objects {
	struct SensorModel {
		double muTarget    = 0.65;
		double sigmaTarget = 0.25;

		double muNoise     = 0.35;
		double sigmaNoise  = 0.25;

		double singlePulseDetectProb = 0.3; // вероятность обнаружения за 1 импульс
	};

	struct Cell {
		bool   realTarget;
	};

	class Grid {
		unsigned targetCount = 0;
		std::mt19937 _gen;
	public:
		std::vector<std::vector<Cell>> coords;
		SensorModel model;

		Grid();
		Grid(int row, int col);

		void toggleTarget(int r, int c);
		double measure(int row, int col);
		unsigned getTargetCount() const { return targetCount; }

		inline double computeDetectionProb(int n) const {
			return 1.0 - std::pow(1.0 - model.singlePulseDetectProb, n);
		}
	};
}