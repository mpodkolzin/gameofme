#include "cell_automaton/cellular_automaton.hpp"
#include "rules/conway_rule.hpp"
#include "patterns/patterns_library.hpp"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <functional>
#include <string>
#include <cstdlib>
#include <cmath>

using namespace cell_automaton;

// ============================================================================
// Benchmark Configuration
// ============================================================================
struct BenchmarkConfig {
    std::string name;
    int generations;
    bool verbose;
    int warmup_runs;
    int benchmark_runs;
    
    BenchmarkConfig(const std::string& n, int gen = 100, bool v = false, int warmup = 1, int runs = 5)
        : name(n), generations(gen), verbose(v), warmup_runs(warmup), benchmark_runs(runs) {}
};

// ============================================================================
// Benchmark Results
// ============================================================================
struct BenchmarkResult {
    std::string name;
    int generations;
    double avg_time_ms;
    double min_time_ms;
    double max_time_ms;
    double std_dev_ms;
    int final_chunks;
    double generations_per_second;
    double cells_evaluated_estimate;
    
    void print() const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "╭─────────────────────────────────────────────────────────────────╮\n";
        std::cout << "│ " << std::setw(63) << std::left << name << " │\n";
        std::cout << "├─────────────────────────────────────────────────────────────────┤\n";
        std::cout << "│ Generations:     " << std::setw(10) << generations << "                        │\n";
        std::cout << "│ Average time:    " << std::setw(10) << avg_time_ms << " ms                  │\n";
        std::cout << "│ Min/Max time:    " << std::setw(10) << min_time_ms << " / " << std::setw(10) << max_time_ms << " ms        │\n";
        std::cout << "│ Std deviation:   " << std::setw(10) << std_dev_ms << " ms                  │\n";
        std::cout << "│ Final chunks:    " << std::setw(10) << final_chunks << "                        │\n";
        std::cout << "│ Gen/sec:         " << std::setw(10) << generations_per_second << "                        │\n";
        std::cout << "│ Est cells/gen:   " << std::setw(10) << (int)cells_evaluated_estimate << "                        │\n";
        std::cout << "╰─────────────────────────────────────────────────────────────────╯\n\n";
    }
};

// ============================================================================
// Benchmark Runner
// ============================================================================
class BenchmarkRunner {
private:
    std::vector<BenchmarkResult> results;
    
    double calculate_std_dev(const std::vector<double>& times, double mean) {
        double sum_sq_diff = 0.0;
        for (double time : times) {
            double diff = time - mean;
            sum_sq_diff += diff * diff;
        }
        return std::sqrt(sum_sq_diff / times.size());
    }
    
public:
    template<typename InitFunc>
    BenchmarkResult run_benchmark(const BenchmarkConfig& config, InitFunc init_pattern) {
        std::vector<double> times;
        times.reserve(config.benchmark_runs);
        
        int final_chunks = 0;
        
        // Warmup runs
        if (config.verbose) {
            std::cout << "Running " << config.warmup_runs << " warmup runs for " << config.name << "...\n";
        }
        
        for (int warmup = 0; warmup < config.warmup_runs; ++warmup) {
            auto rule = std::make_unique<rules::ConwayRule>();
            CellularAutomaton<bool> ca(std::move(rule), false);
            init_pattern(ca);
            ca.run(config.generations);
        }
        
        // Actual benchmark runs
        if (config.verbose) {
            std::cout << "Running " << config.benchmark_runs << " benchmark runs for " << config.name << "...\n";
        }
        
        for (int run = 0; run < config.benchmark_runs; ++run) {
            auto rule = std::make_unique<rules::ConwayRule>();
            CellularAutomaton<bool> ca(std::move(rule), false);
            init_pattern(ca);
            
            auto start = std::chrono::high_resolution_clock::now();
            ca.run(config.generations);
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            times.push_back(duration.count() / 1000.0); // Convert to milliseconds
            
            final_chunks = ca.get_active_chunks();
            
            if (config.verbose) {
                std::cout << "  Run " << (run + 1) << ": " << times.back() << " ms\n";
            }
        }
        
        // Calculate statistics
        double sum = 0.0;
        double min_time = times[0];
        double max_time = times[0];
        
        for (double time : times) {
            sum += time;
            min_time = std::min(min_time, time);
            max_time = std::max(max_time, time);
        }
        
        double avg_time = sum / times.size();
        double std_dev = calculate_std_dev(times, avg_time);
        double gen_per_sec = 1000.0 * config.generations / avg_time;
        
        // Rough estimate of cells evaluated per generation
        double cells_estimate = final_chunks * 64 * 64 * 9; // chunks * cells * neighbors
        
        BenchmarkResult result;
        result.name = config.name;
        result.generations = config.generations;
        result.avg_time_ms = avg_time;
        result.min_time_ms = min_time;
        result.max_time_ms = max_time;
        result.std_dev_ms = std_dev;
        result.final_chunks = final_chunks;
        result.generations_per_second = gen_per_sec;
        result.cells_evaluated_estimate = cells_estimate;
        
        results.push_back(result);
        return result;
    }
    
    void print_summary() {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "BENCHMARK SUMMARY\n";
        std::cout << std::string(80, '=') << "\n\n";
        
        for (const auto& result : results) {
            result.print();
        }
        
        // Performance comparison
        if (results.size() > 1) {
            std::cout << "Performance Comparison (generations/second):\n";
            std::cout << std::string(50, '-') << "\n";
            
            auto fastest = std::max_element(results.begin(), results.end(),
                [](const auto& a, const auto& b) { return a.generations_per_second < b.generations_per_second; });
            
            for (const auto& result : results) {
                double relative = result.generations_per_second / fastest->generations_per_second;
                std::cout << std::setw(30) << std::left << result.name 
                         << std::setw(10) << std::fixed << std::setprecision(1) << result.generations_per_second 
                         << " (" << std::setprecision(2) << (relative * 100) << "%)\n";
            }
            std::cout << "\n";
        }
    }
};

// ============================================================================
// Pattern Initialization Functions
// ============================================================================
auto init_small_soup = [](CellularAutomaton<bool>& ca) {
    std::srand(42);
    patterns::create_random_soup(ca, -25, -25, 50, 50, 0.3);
};

auto init_medium_soup = [](CellularAutomaton<bool>& ca) {
    std::srand(42);
    patterns::create_random_soup(ca, -100, -100, 200, 200, 0.25);
};

auto init_large_soup = [](CellularAutomaton<bool>& ca) {
    std::srand(42);
    patterns::create_random_soup(ca, -250, -250, 500, 500, 0.2);
};

auto init_sparse_pattern = [](CellularAutomaton<bool>& ca) {
    std::srand(42);
    patterns::create_random_soup(ca, -500, -500, 1000, 1000, 0.05);
};

auto init_dense_pattern = [](CellularAutomaton<bool>& ca) {
    std::srand(42);
    patterns::create_random_soup(ca, -50, -50, 100, 100, 0.8);
};

auto init_glider_fleet = [](CellularAutomaton<bool>& ca) {
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 20; ++j) {
            patterns::create_glider(ca, i * 10, j * 10);
        }
    }
};

auto init_r_pentomino = [](CellularAutomaton<bool>& ca) {
    patterns::create_r_pentomino(ca, 0, 0);
};

auto init_gosper_gun = [](CellularAutomaton<bool>& ca) {
    patterns::create_gosper_glider_gun(ca, 0, 0);
};

// ============================================================================
// Main Benchmark Suite
// ============================================================================
int main(int argc, char* argv[]) {
    bool verbose = argc > 1 && std::string(argv[1]) == "-v";
    
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              CELLULAR AUTOMATON BENCHMARK SUITE               ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    BenchmarkRunner runner;
    
    // Quick benchmarks
    runner.run_benchmark(BenchmarkConfig("R-Pentomino (1000 gen)", 1000, verbose), init_r_pentomino);
    runner.run_benchmark(BenchmarkConfig("Small Random Soup (100 gen)", 100, verbose), init_small_soup);
    runner.run_benchmark(BenchmarkConfig("Glider Fleet (100 gen)", 100, verbose), init_glider_fleet);
    
    // Stress tests
    runner.run_benchmark(BenchmarkConfig("Medium Random Soup (50 gen)", 50, verbose), init_medium_soup);
    runner.run_benchmark(BenchmarkConfig("Large Sparse Pattern (20 gen)", 20, verbose), init_sparse_pattern);
    runner.run_benchmark(BenchmarkConfig("Dense Pattern (50 gen)", 50, verbose), init_dense_pattern);
    
    // Long-running tests
    runner.run_benchmark(BenchmarkConfig("Gosper Gun (500 gen)", 500, verbose), init_gosper_gun);
    runner.run_benchmark(BenchmarkConfig("Large Random Soup (10 gen)", 10, verbose), init_large_soup);
    
    runner.print_summary();
    
    return 0;
}