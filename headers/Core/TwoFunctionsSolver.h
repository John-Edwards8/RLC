#pragma once
#include "ISolver.h"
#include <cmath>
#include <iostream>

namespace Core {
    class TwoFunctionsSolver : public ISolver {
    public:
        TwoFunctionsSolver(int rows, int cols,
            int maxImpulses = 10000,
            Objects::SensorModel model = {})
            : ISolver(rows, cols, maxImpulses, model)
        {}

        std::pair<int, int> chooseCell() override;
        void onSignalResult(int row, int col, double signal) override;
        bool finished() const override;

    private:
        double calculateGain(int r, int c) const;
    };
}