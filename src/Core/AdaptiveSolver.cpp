#include "../../headers/Core/AdaptiveSolver.h"

namespace Core {
	std::pair<int, int> AdaptiveSolver::chooseCell() {
		if (firstPassNotFinished())
			return nextSequentialCell();

        double bestScore = -1e18;
        int bestR = 0;
        int bestC = 0;
        double std = globalStd();

        for (int r = 0; r < rows; r++)
            for (int c = 0; c < cols; c++) {
                auto& cell = stats[r][c];

                double exploitation = cell.meanSignal;
                double exploration = C * sqrt(log(totalSteps + 1) / (cell.visits + 1));
                double score = exploitation + exploration;

                if (score > bestScore) {
                    bestScore = score;
                    bestR = r;
                    bestC = c;
                }
            }

        std::cout << "Chosen: (" << bestR + 1 << "," << bestC + 1
            << ") score=" << bestScore << std::endl;

        return { bestR,bestC };
	}

	void AdaptiveSolver::onSignalResult(int row, int col, double signal) {
        auto& cell = stats[row][col];

        cell.visits++;

        double deltaLocal = signal - cell.meanSignal;
        cell.meanSignal += deltaLocal / cell.visits;
        cell.variance += deltaLocal * (signal - cell.meanSignal);

        if (cell.meanSignal > bestMeanEver)
            bestMeanEver = cell.meanSignal;

        totalSteps++;

        globalCount++;

        double deltaGlobal = signal - globalMean;
        globalMean += deltaGlobal / globalCount;
        globalVar += deltaGlobal * (signal - globalMean);

        if (bestMeanEver > lastBestMean + 1e-6) {
            globalStable = 0;
            lastBestMean = bestMeanEver;
        } else {
            globalStable++;
        }

        std::cout
            << "step " << totalSteps
            << " best=" << bestMeanEver
            << " stable=" << globalStable
            << std::endl;
	}

    bool AdaptiveSolver::finished() const {
        if (totalSteps >= maxSteps)
            return true;

        if (globalStable > maxSteps * (2.0/3.0))
            return true;

        return false;
    }
}