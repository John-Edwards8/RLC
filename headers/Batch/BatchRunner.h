#pragma once
#include "Objects.h"
#include "Core/ISolver.h"
#include "sqlite3.h"
#include <vector>
#include <string>
#include <random>

struct BatchConfig {
    int    runs = 100;
    double duration = 1.0;
    int    frequency = 300;
};

class BatchRunner {
    BatchConfig _cfg;
    sqlite3* _db = nullptr;

    static std::vector<std::pair<int, int>> randomTargets(
        int rows, int cols, int count, std::mt19937& rng);

    void initDB();

    void runOnce(Core::SolverType algo, int runIndex,
        int rows, int cols, double duration, int frequency,
        const std::vector<std::pair<int, int>>& targets,
        const std::string& scenarioTag);

    void runScenario(Core::SolverType algo,
        int rows, int cols, int targetCount, double duration, int frequency,
        const std::string& scenarioTag,
        std::mt19937& rng);

    int  insertRun(const std::string& algo, int runIndex,
        const std::string& scenario,
        int targets, int found, int falsePos,
        int totalImpulses,
        int impulsesToLastDet,
        double impDetAvg);

    void insertEvent(int runId, int row, int col,
        int impulses, double tDetect, bool isCorrect);
public:
    explicit BatchRunner(BatchConfig cfg,
        const std::string& dbPath = "results.db");
    ~BatchRunner();
    void run();
};