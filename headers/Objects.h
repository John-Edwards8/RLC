#pragma once
#include <random>
#include <iostream>

namespace Objects {
	struct Cell {
		bool realTarget;
		double belief = 0.0;
	};

	class Grid {
		unsigned cellsInRow, cellsInColumn;
		unsigned targetCount = 0;
	public:
		std::vector<std::vector<Cell>> coords;

		Grid();
		Grid(int row, int col);
		~Grid();

		double measure(int row, int col);
		unsigned getTargetCount() const { return targetCount; }

		[[deprecated]] void setValues(unsigned scrW, unsigned scrH, unsigned cellsInRow, unsigned cellsInColumn);
	};
}