#pragma once
#include <random>
#include <iostream>

namespace Objects {
	/*struct SensorModel {
		double muTarget    = 0.65;
		double sigmaTarget = 0.25;

		double muNoise     = 0.35;
		double sigmaNoise  = 0.25;
	};*/

	struct SensorModel {
		double muTarget = 5.0;   // Подняли до уровня реального SNR
		double sigmaTarget = 1.0;

		double muNoise = 0.0;   // Нормируем шум к нулю
		double sigmaNoise = 1.0;

		double targetPfa = 1e-6;  // Та самая одна миллионная
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
	};
}