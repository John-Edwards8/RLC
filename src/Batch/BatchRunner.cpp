#include "Batch/BatchRunner.h"
#include "Core/SequentialSolver.h"
#include "Core/MaxElementSolver.h"
#include "Core/TwoFunctionsSolver.h"
#include "Core/WeightedCoefficientsSolver.h"
#include "Objects.h"
#include <iostream>
#include <random>
#include <stdexcept>

BatchRunner::BatchRunner(BatchConfig cfg, const std::string& dbPath)
    : _cfg(std::move(cfg))
{
    if (sqlite3_open(dbPath.c_str(), &_db) != SQLITE_OK)
        throw std::runtime_error("Cannot open DB: " + std::string(sqlite3_errmsg(_db)));
    initDB();
}

BatchRunner::~BatchRunner() {
    if (_db) sqlite3_close(_db);
}

void BatchRunner::initDB() {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS runs (
            id             INTEGER PRIMARY KEY AUTOINCREMENT,
            algo           TEXT,
            run_index      INTEGER,
            targets        INTEGER,
            found          INTEGER,
            false_pos      INTEGER,
            total_impulses INTEGER,
            t_detect_avg   REAL
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

void BatchRunner::runOnce(Core::SolverType algo, int runIndex) {
    Objects::Grid grid(_cfg.rows, _cfg.cols);

    // расставить цели
    if (_cfg.targets.empty()) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<> dr(0, _cfg.rows - 1);
        std::uniform_int_distribution<> dc(0, _cfg.cols - 1);
        grid.toggleTarget(dr(rng), dc(rng));
    }
    else {
        for (auto [r, c] : _cfg.targets)
            grid.toggleTarget(r, c);
    }

    std::unique_ptr<Core::ISolver> solver;
    std::string algoName;
    if (algo == Core::SolverType::SEQUENTIAL) {
        solver = std::make_unique<Core::SequentialSolver>(
            _cfg.rows, _cfg.cols, _cfg.impulses, grid.model);
        algoName = "SEQUENTIAL";
    }
    else if (algo == Core::SolverType::MAX_ELEMENT) {
        solver = std::make_unique<Core::MaxElementSolver>(
            _cfg.rows, _cfg.cols, _cfg.impulses, grid.model);
        algoName = "MAX_ELEMENT";
    }
    else if (algo == Core::SolverType::TWO_FUNCTIONS) {
        solver = std::make_unique<Core::TwoFunctionsSolver>(
            _cfg.rows, _cfg.cols, _cfg.impulses, grid.model);
        algoName = "TWO_FUNCTIONS";
    }
    else if (algo == Core::SolverType::WEIGHTED_COEFFICIENTS) {
        solver = std::make_unique<Core::WeightedCoefficientsSolver>(
            _cfg.rows, _cfg.cols, _cfg.impulses, grid.model);
		algoName = "WEIGHTED_COEFFICIENTS";
    }
    
    struct Event { int row, col, impulses; bool isCorrect; };
    std::vector<Event> events;
    std::vector<std::vector<bool>> alreadyDetected(
        _cfg.rows, std::vector<bool>(_cfg.cols, false));

    while (!solver->finished()) {
        auto [row, col] = solver->chooseCell();
        double signal = grid.measure(row, col);
        solver->onSignalResult(row, col, signal);

        if (solver->getRecentPositives(row, col) >= 2
            && solver->getBelief(row, col) >= 0.7
            && !alreadyDetected[row][col])
        {
            alreadyDetected[row][col] = true;
            events.push_back({ row, col, solver->getTotalImpulses(),
                                grid.coords[row][col].realTarget });
            solver->markDecided(row, col);
        }
    }

    int found = 0, falsePos = 0;
    double tSum = 0.0;
    for (auto& e : events) {
        if (e.isCorrect) found++;
        else falsePos++;
        tSum += static_cast<double>(e.impulses) / _cfg.frequency;
    }
    double tAvg = events.empty() ? 0.0 : tSum / events.size();

    int runId = insertRun(algoName, runIndex,
        grid.getTargetCount(), found, falsePos,
        solver->getTotalImpulses(), tAvg);

    for (auto& e : events)
        insertEvent(runId, e.row, e.col, e.impulses,
            static_cast<double>(e.impulses) / _cfg.frequency,
            e.isCorrect);
}

void BatchRunner::run() {
    const std::vector<Core::SolverType> algos = {
        Core::SolverType::SEQUENTIAL,
        Core::SolverType::MAX_ELEMENT,
        Core::SolverType::TWO_FUNCTIONS,
		Core::SolverType::WEIGHTED_COEFFICIENTS
    };
    for (auto algo : algos) {
        std::string name = (algo == Core::SolverType::SEQUENTIAL) ?
            "SEQUENTIAL" : (algo == Core::SolverType::MAX_ELEMENT) ?
            "MAX_ELEMENT" : (algo == Core::SolverType::TWO_FUNCTIONS) ?
            "TWO_FUNCTIONS" : "WEIGHTED_COEFFICIENTS";
        std::cout << "Running " << name << " x" << _cfg.runs << "...\n";
        for (int i = 0; i < _cfg.runs; i++)
            runOnce(algo, i);
        std::cout << "  done\n";
    }
    std::cout << "Batch complete. Results in results.db\n";
}

int BatchRunner::insertRun(const std::string& algo, int runIndex,
    int targets, int found, int falsePos,
    int totalImpulses, double tDetectAvg)
{
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(_db,
        "INSERT INTO runs(algo,run_index,targets,found,false_pos,total_impulses,t_detect_avg)"
        " VALUES(?,?,?,?,?,?,?)", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, algo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, runIndex);
    sqlite3_bind_int(stmt, 3, targets);
    sqlite3_bind_int(stmt, 4, found);
    sqlite3_bind_int(stmt, 5, falsePos);
    sqlite3_bind_int(stmt, 6, totalImpulses);
    sqlite3_bind_double(stmt, 7, tDetectAvg);
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
        " VALUES(?,?,?,?,?,?)", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, runId);
    sqlite3_bind_int(stmt, 2, row);
    sqlite3_bind_int(stmt, 3, col);
    sqlite3_bind_int(stmt, 4, impulses);
    sqlite3_bind_double(stmt, 5, tDetect);
    sqlite3_bind_int(stmt, 6, isCorrect ? 1 : 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}