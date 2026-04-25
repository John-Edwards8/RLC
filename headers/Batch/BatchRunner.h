#pragma once
#include "Objects.h"
#include "Core/ISolver.h"
#include "sqlite3.h"
#include <vector>
#include <string>

struct BatchConfig {
    int runs = 100;
    int rows = 5;
    int cols = 5;
    int impulses = 10000;
    int frequency = 100;
    std::vector<std::pair<int, int>> targets; // пусто = рандом 1 цель
};

class BatchRunner {
    BatchConfig _cfg;
    sqlite3* _db = nullptr;

    void initDB();
    void runOnce(Core::SolverType algo, int runIndex);
    int  insertRun(const std::string& algo, int runIndex,
        int targets, int found, int falsePos,
        int totalImpulses, double tDetectAvg);
    void insertEvent(int runId, int row, int col,
        int impulses, double tDetect, bool isCorrect);
public:
    explicit BatchRunner(BatchConfig cfg, const std::string& dbPath = "results.db");
    ~BatchRunner();
    void run();
};