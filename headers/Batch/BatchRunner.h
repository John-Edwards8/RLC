#pragma once
#include "Objects.h"
#include "Core/ISolver.h"
#include "sqlite3.h"
#include <vector>
#include <string>
#include <random>

struct BatchConfig {
    int    runs = 1000;
    double duration = 0.6;
    int    frequency = 10000;
};

class BatchRunner {
    BatchConfig _cfg;
    sqlite3* _db = nullptr;
    sqlite3_stmt* _insertRunStmt = nullptr;
    sqlite3_stmt* _insertEventStmt = nullptr;

    struct EventRec {
        int row, col, impulses;
        double tDetect;
        bool isCorrect;
    };
    struct RunResult {
        std::string algo, scenario;
        int runIndex, targets, found, falsePos, totalImpulses, impulsesToLastDet;
        double impDetAvg;
        std::vector<EventRec> events;
    };

    static std::vector<std::pair<int, int>> randomTargets(
        int rows, int cols, int count, std::mt19937& rng);

    void initDB();

    RunResult computeOnce(Core::SolverType algo, int runIndex,
        int rows, int cols, double duration, int frequency,
        const std::vector<std::pair<int, int>>& targets,
        const std::string& scenarioTag);

    void runOnce(Core::SolverType algo, int runIndex,
        int rows, int cols, double duration, int frequency,
        const std::vector<std::pair<int, int>>& targets,
        const std::string& scenarioTag);

    void runScenario(Core::SolverType algo,
        int rows, int cols, int targetCount, double duration, int frequency,
        const std::string& scenarioTag,
        std::mt19937& rng);

    void persistResult(const RunResult& r);

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