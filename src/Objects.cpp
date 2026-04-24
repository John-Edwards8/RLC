#include "../headers/Objects.h"

namespace Objects {
	Grid::Grid() : Grid(5, 5) {}

	Grid::Grid(int row, int col) {
		std::random_device rd;
		_gen = std::mt19937(rd());

		coords.resize(row);
		for (auto& r : coords)
			r.resize(col);

		// цели не расставляем — ждём пользователя
		for (int i = 0; i < row; i++)
			for (int j = 0; j < col; j++) {
				coords[i][j].realTarget = false;
			}
	}

	void Grid::toggleTarget(int r, int c) {
		bool& t = coords[r][c].realTarget;
		t = !t;
		targetCount += t ? 1 : -1;
	}

	double Grid::measure(int r, int c) {
		if (coords[r][c].realTarget) {
			std::normal_distribution<> dist(model.muTarget, model.sigmaTarget);
			return std::max(0.0, std::min(1.0, dist(_gen)));
		}
		else {
			std::normal_distribution<> dist(model.muNoise, model.sigmaNoise);
			return std::max(0.0, std::min(1.0, dist(_gen)));
		}
	}
}