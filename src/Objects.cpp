#include "../headers/Objects.h"

namespace Objects {
	Grid::Grid() : Grid(5, 5) {}

	Grid::Grid(int row, int col) : cellsInRow(row), cellsInColumn(col),
								   _gen(std::random_device{}())
	{
		std::bernoulli_distribution dist(0.1);

		coords.resize(cellsInRow);
		for (auto& row : coords) {
			row.resize(cellsInColumn);
		}
		
		for (int i = 0; i < cellsInRow; i++) {
			for (int j = 0; j < cellsInColumn; j++) {
				coords[i][j].realTarget = dist(_gen);
				if (coords[i][j].realTarget) {
					targetCount++;
					std::cout << "Target is realy in (" << i + 1 << "," <<j + 1 << ")." << std::endl;
				}
			}
		}
	}

	Grid::~Grid() {
		coords.clear();
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

	[[deprecated("The grid is set once. Changing the grid = recreating the object.")]]
	void Grid::setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn) {
		cellsInRow = cellsInRow;
		cellsInColumn = cellsInColumn;
	}
}