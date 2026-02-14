#pragma once
#include <vector>

namespace Render {
	struct CellRenderInfo {
		float x, y;
		int size;
		double confidence;
		bool isTarget;
	};

	struct BeamRenderInfo {
		float x, y;
		float radius;
	};

	struct SceneData {
		std::vector<CellRenderInfo> cells;
		BeamRenderInfo beam;
		bool beamActive = false;

		int rows = 0;
		int cols = 0;
		int cellSize = 0;
		int borderX = 0;
		int borderY = 0;
	};
}