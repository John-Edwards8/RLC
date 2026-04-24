#pragma once
#include <vector>

enum class AppState { SETUP, RUNNING, FINISHED };

namespace Render {
	struct CellRenderInfo {
		float x, y, size;
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

		int rows = 0;
		int cols = 0;
		int cellSize = 0;
		int borderX = 0;
		int borderY = 0;

		AppState state = AppState::SETUP;
	};
}