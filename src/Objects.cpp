#include "../headers/Objects.h"

namespace Objects {
	Grid::Grid() : Grid(5, 5) {}

	Grid::Grid(int row, int col) : cellsInRow(row), cellsInColumn(col) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::bernoulli_distribution dist(0.1);

		coords.resize(cellsInRow);
		for (auto& row : coords) {
			row.resize(cellsInColumn);
		}
		
		for (int i = 0; i < cellsInRow; i++) {
			for (int j = 0; j < cellsInColumn; j++) {
				coords[i][j].realTarget = dist(gen);
				if (coords[i][j].realTarget) {
					targetCount++;
					std::cout << "Target is realy in (" << i + 1 << "," <<j + 1 << ")." << std::endl;
				}
				coords[i][j].belief = 0.0;
			}
		}
	}

	Grid::~Grid() {
		coords.clear();
	}

	double Grid::measure(int r, int c) {
		std::random_device rd;
		std::mt19937 gen(rd());

		if (coords[r][c].realTarget) {
			// Target: strong signal with low noise
			std::normal_distribution<> dist(0.8, 0.1);		// mean=0.8, std=0.1
			return std::max(0.5, std::min(1.0, dist(gen)));
		} else {
			// Non-target: faint noise
			std::uniform_real_distribution<> dist(0.0, 0.2);
			return dist(gen);
		}
	}

	[[deprecated("The grid is set once. Changing the grid = recreating the object.")]]
	void Grid::setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn) {
		cellsInRow = cellsInRow;
		cellsInColumn = cellsInColumn;
	}
}