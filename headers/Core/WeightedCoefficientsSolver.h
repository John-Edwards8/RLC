#pragma once
#include "ISolver.h"

namespace Core {
    class WeightedCoefficientsSolver : public ISolver {
    public:
        WeightedCoefficientsSolver(int rows, int cols,
            int maxImpulses = 10000,
            Objects::SensorModel model = {})
            : ISolver(rows, cols, maxImpulses, model)
        {
        }

        std::pair<int, int> chooseCell() override;
        void onSignalResult(int row, int col, double signal) override;
        bool finished() const override;

    private:
        double calculateWeight(int r, int c) const;
    };
}