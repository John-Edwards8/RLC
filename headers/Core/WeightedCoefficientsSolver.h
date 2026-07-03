#pragma once
#include "ISolver.h"

namespace Core {
    class WeightedCoefficientsSolver : public ISolver {
    public:
        WeightedCoefficientsSolver(int rows, int cols,
            Objects::SensorModel model = {})
            : ISolver(rows, cols, model)
        {
        }

        //std::pair<int, int> chooseCell() override;
        //void onSignalResult(int row, int col, double signal) override;
        void buildPlan(int budget) override;
    private:
        double calculateWeight(int r, int c) const;
    };
}