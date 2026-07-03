#pragma once
#include "ISolver.h"
#include <cmath>
#include <iostream>

namespace Core {
    class TwoFunctionsSolver : public ISolver {
    public:
        TwoFunctionsSolver(int rows, int cols,
            Objects::SensorModel model = {})
            : ISolver(rows, cols, model)
        {}

        //std::pair<int, int> chooseCell() override;
        //void onSignalResult(int row, int col, double signal) override;
        void buildPlan(int budget) override;
    private:
        double calculateGain(int r, int c) const;
    };
}