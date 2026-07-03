#pragma once
#include <vector>

enum class AppState { SETUP, COMPUTING, REPLAY, FINISHED };

namespace Render {
	//struct CellRenderInfo {
	//	float x, y, size;
	//	double confidence;
	//	bool isTarget;
	//};
	struct CellRenderInfo {
		float  x, y, size;
		double confidence;
		bool   isTarget;
		int    planAllocation = 0; // сколько импульсов назначено в текущем плане
		int    planVisitOrder = 0; // порядок первого посещения (0 = не назначена)
		int    planVisitRank = 0;
		bool   isRevealed = false; // цель показана (луч уже был здесь)
		bool   isDetected = false; // алгоритм детектировал
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

	struct FrameSnapshot {
		std::vector<CellRenderInfo> cells;
		BeamRenderInfo beam;
		int impulseIndex = 0;
		int detectedRow = -1;
		int detectedCol = -1;
		int sweepNumber = 0;
	};
}