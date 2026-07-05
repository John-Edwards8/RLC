#include "Batch/BatchRunner.h"
#include "Core/SequentialSolver.h"
#include "Core/MaxElementSolver.h"
#include "Core/TwoFunctionsSolver.h"
#include "Core/WeightedCoefficientsSolver.h"
#include "Objects.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static std::string algoString(Core::SolverType algo) {
    switch (algo) {
    case Core::SolverType::SEQUENTIAL:            return "SEQUENTIAL";
    case Core::SolverType::MAX_ELEMENT:           return "MAX_ELEMENT";
    case Core::SolverType::TWO_FUNCTIONS:         return "TWO_FUNCTIONS";
    case Core::SolverType::WEIGHTED_COEFFICIENTS: return "WEIGHTED_COEFFICIENTS";
    }
    return "UNKNOWN";
}

/*static*/ std::vector<std::pair<int, int>>
BatchRunner::randomTargets(int rows, int cols, int count, std::mt19937& rng) {
    std::vector<std::pair<int, int>> pool;
    pool.reserve(rows * cols);
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            pool.push_back({ r, c });
    std::shuffle(pool.begin(), pool.end(), rng);
    pool.resize(count);
    return pool;
}

// ---------------------------------------------------------------------------
// Construction / DB init
// ---------------------------------------------------------------------------

BatchRunner::BatchRunner(BatchConfig cfg, const std::string& dbPath)
    : _cfg(std::move(cfg))
{
    if (sqlite3_open(dbPath.c_str(), &_db) != SQLITE_OK)
        throw std::runtime_error("Cannot open DB: " +
            std::string(sqlite3_errmsg(_db)));
    initDB();
}

BatchRunner::~BatchRunner() {
    if (_db) sqlite3_close(_db);
}

void BatchRunner::initDB() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS runs (
            id                    INTEGER PRIMARY KEY AUTOINCREMENT,
            algo                  TEXT,
            scenario              TEXT,
            run_index             INTEGER,
            targets               INTEGER,
            found                 INTEGER,
            false_pos             INTEGER,
            total_impulses        INTEGER,
            impulses_to_last_det  INTEGER,
            impulses_det_avg      REAL
        );
        CREATE TABLE IF NOT EXISTS events (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            run_id     INTEGER REFERENCES runs(id),
            row        INTEGER,
            col        INTEGER,
            impulses   INTEGER,
            t_detect   REAL,
            is_correct INTEGER
        );
    )";
    char* err = nullptr;
    if (sqlite3_exec(_db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err; sqlite3_free(err);
        throw std::runtime_error("DB init failed: " + msg);
    }
}

// ---------------------------------------------------------------------------
// Single run
// ---------------------------------------------------------------------------

void BatchRunner::runOnce(Core::SolverType algo, int runIndex,
	int rows, int cols, double duration, int frequency,
    const std::vector<std::pair<int, int>>& targets,
    const std::string& scenarioTag)
{
    Objects::Grid grid(rows, cols);
    for (auto [r, c] : targets)
        grid.toggleTarget(r, c);

    const int sweepBudget = static_cast<int>(duration * frequency);
    const std::string algoName = algoString(algo);

    std::unique_ptr<Core::ISolver> solver;
    switch (algo) {
    case Core::SolverType::SEQUENTIAL:
        solver = std::make_unique<Core::SequentialSolver>(rows, cols, grid.model);
        break;
    case Core::SolverType::MAX_ELEMENT:
        solver = std::make_unique<Core::MaxElementSolver>(rows, cols, grid.model);
        break;
    case Core::SolverType::TWO_FUNCTIONS:
        solver = std::make_unique<Core::TwoFunctionsSolver>(rows, cols, grid.model);
        break;
    case Core::SolverType::WEIGHTED_COEFFICIENTS:
        solver = std::make_unique<Core::WeightedCoefficientsSolver>(rows, cols, grid.model);
        break;
    }

    struct Event { int row, col; int impulses; bool isCorrect; };
    std::vector<Event> events;
    std::vector<bool>  alreadyDetected(rows * cols, false);

    // Основной цикл — идентичен Manager::compute()
    while (!solver->finished()) {
        auto [row, col] = solver->chooseCell(sweepBudget);
        double signal = grid.measure(row, col);
        solver->onSignalResult(row, col, signal);

        int idx = row * cols + col;
        if (solver->getRecentPositives(row, col) >= 2
            && solver->getBelief(row, col) >= 0.9
            && !alreadyDetected[idx])
        {
            alreadyDetected[idx] = true;
            bool correct = grid.coords[row][col].realTarget;

            // Фиксируем getTotalImpulses() ЗДЕСЬ — в момент срабатывания
            // критерия обнаружения, а не после окончания всех прогонов солвера.
            events.push_back({ row, col, solver->getTotalImpulses(), correct });
            solver->markDecided(row, col);
        }
    }
    solver->flushPending();

    // --- Подсчёт метрик ---
    int    found = 0;
    int    falsePos = 0;
    int    lastDetImp = 0;   // импульс последней правильной цели
    double impSum = 0.0; // сумма для среднего

    for (auto& e : events) {
        if (e.isCorrect) {
            found++;
            impSum += e.impulses;
            lastDetImp = std::max(lastDetImp, e.impulses);
        }
        else {
            falsePos++;
        }
    }

    // impulses_det_avg  — среднее число импульсов до обнаружения одной правильной цели
    // impulses_to_last_det — импульс, когда была обнаружена ПОСЛЕДНЯЯ цель (аналог T_search)
    double impDetAvg = (found > 0) ? impSum / found : 0.0;

    int runId = insertRun(algoName, runIndex, scenarioTag,
        (int)targets.size(), found, falsePos,
        solver->getTotalImpulses(),
        lastDetImp,
        impDetAvg);

    for (auto& e : events)
        insertEvent(runId, e.row, e.col, e.impulses,
            static_cast<double>(e.impulses) / _cfg.frequency,
            e.isCorrect);
}

// ---------------------------------------------------------------------------
// Scenario runner: 100 прогонов, цели переставляются каждые 10 прогонов
// ---------------------------------------------------------------------------

void BatchRunner::runScenario(Core::SolverType algo,
	int rows, int cols, int targetCount, double duration, int frequency,
    const std::string& scenarioTag,
    std::mt19937& rng)
{
    const std::string algoName = algoString(algo);
    std::cout << "  [" << scenarioTag << "] " << algoName
        << " - " << _cfg.runs << " runs, "
        << rows << "x" << cols << ", " << targetCount << " target(s)\n";

    auto currentTargets = randomTargets(rows, cols, targetCount, rng);

    for (int i = 0; i < _cfg.runs; i++) {
        if (i > 0 && i % 10 == 0) {
            currentTargets = randomTargets(rows, cols, targetCount, rng);
            std::cout << "    run " << i << ": relocated " << targetCount
                << " target(s)\n";
        }
        runOnce(algo, i, rows, cols, duration, frequency, currentTargets, scenarioTag);
    }
    std::cout << "    done\n";
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

void BatchRunner::run() {
    std::mt19937 rng(std::random_device{}());

    const std::vector<Core::SolverType> algos = {
        Core::SolverType::SEQUENTIAL,
        Core::SolverType::MAX_ELEMENT,
        Core::SolverType::TWO_FUNCTIONS,
        Core::SolverType::WEIGHTED_COEFFICIENTS
    };

    std::cout << "=== BATCH START ===\n";
    for (auto algo : algos) {
        runScenario(algo, 30, 100, 1, 0.6, 10000, "1_TARGET", rng);
        runScenario(algo, 30, 100, 20, 0.6, 10000, "20_TARGETS", rng);
    }
    std::cout << "=== BATCH COMPLETE. Results -> results.db ===\n";
}

// ---------------------------------------------------------------------------
// DB helpers
// ---------------------------------------------------------------------------

int BatchRunner::insertRun(const std::string& algo, int runIndex,
    const std::string& scenario,
    int targets, int found, int falsePos,
    int totalImpulses,
    int impulsesToLastDet,
    double impDetAvg)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(_db,
        "INSERT INTO runs"
        "(algo,scenario,run_index,targets,found,false_pos,"
        " total_impulses,impulses_to_last_det,impulses_det_avg)"
        " VALUES(?,?,?,?,?,?,?,?,?)",
        -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, algo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, scenario.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, runIndex);
    sqlite3_bind_int(stmt, 4, targets);
    sqlite3_bind_int(stmt, 5, found);
    sqlite3_bind_int(stmt, 6, falsePos);
    sqlite3_bind_int(stmt, 7, totalImpulses);
    sqlite3_bind_int(stmt, 8, impulsesToLastDet);
    sqlite3_bind_double(stmt, 9, impDetAvg);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return static_cast<int>(sqlite3_last_insert_rowid(_db));
}

void BatchRunner::insertEvent(int runId, int row, int col,
    int impulses, double tDetect, bool isCorrect)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(_db,
        "INSERT INTO events(run_id,row,col,impulses,t_detect,is_correct)"
        " VALUES(?,?,?,?,?,?)",
        -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, runId);
    sqlite3_bind_int(stmt, 2, row);
    sqlite3_bind_int(stmt, 3, col);
    sqlite3_bind_int(stmt, 4, impulses);
    sqlite3_bind_double(stmt, 5, tDetect);
    sqlite3_bind_int(stmt, 6, isCorrect ? 1 : 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}