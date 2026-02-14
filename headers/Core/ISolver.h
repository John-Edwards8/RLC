#pragma once
#include <utility>

namespace Core {
	class ISolver {
	public:
		virtual std::pair<int, int> chooseCell() = 0;

		virtual void onSignalResult(int row, int col, double signal) = 0;

		virtual bool finished() const = 0;

		virtual ~ISolver() = default;
	};
}