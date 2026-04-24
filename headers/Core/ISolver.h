#pragma once
#include "Objects.h"
#include <utility>

namespace Core {
	enum class SolverType {
		SEQUENTIAL,
		MAX_ELEMENT
	};

	class ISolver {
	public:
		virtual std::pair<int, int> chooseCell() = 0;
		virtual void markDecided(int r, int c) = 0;
		virtual void onSignalResult(int row, int col, double signal) = 0;
		virtual double getBelief(int r, int c) = 0;
		virtual int getTotalImpulses() const = 0;
		virtual int getRecentPositives(int r, int c) const = 0;
		virtual bool finished() const = 0;

		virtual ~ISolver() = default;

		static double gauss(double x, double mu, double sig) {
			return (1.0 / (sig * 2.5066282746)) *
				std::exp(-0.5 * ((x - mu) / sig) * ((x - mu) / sig));
		}
	};
}