#pragma once
#include "ISolver.h"
#include <vector>
#include <iostream>
#include <numbers>

namespace Core {
	struct CellStats {
		int visits = 0;
		double meanSignal = 0;
		double variance = 0;
	};

	class AdaptiveSolver : public ISolver {
		int rows;
		int cols;

		int curRow = 0;
		int curCol = 0;

		int maxSteps;
		int totalSteps = 0;
		double C = 1.4;

		double bestMeanEver = -1e18;
		int globalStable = 0;
		double lastBestMean = 0;
		double detectionThreshold = 0.9;

		double globalMean = 0.0;
		double globalVar = 0.0;
		int globalCount = 0;

		std::vector<std::vector<CellStats>> stats;
	public:
		AdaptiveSolver(int r, int c) : rows(r), cols(c), maxSteps(r * c * 3) {
			stats.resize(rows);
			for (auto& row : stats)
				row.resize(cols);
		}

		std::pair<int, int> chooseCell() override;
		void onSignalResult(int row, int col, double signal) override;
		bool finished() const override;

		bool firstPassNotFinished() const {
			return totalSteps < rows * cols;
		}

		double globalStd() const {
			if (globalCount < 2)
				return 1.0;
			return sqrt(globalVar / (globalCount - 1));
		}

		std::pair<int, int> nextSequentialCell() {
			int r = curRow;
			int c = curCol;

			curCol++;
			if (curCol >= cols) {
				curCol = 0;
				curRow++;
			}

			return { r,c };
		}

		~AdaptiveSolver() {}
	};
}