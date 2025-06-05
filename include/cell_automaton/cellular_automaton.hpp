#ifndef CELLULAR_AUTOMATON_HPP
#define CELLULAR_AUTOMATON_HPP

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <memory>
#include <tuple>
#include <iostream>
#include "rules/rule_base.hpp"


using namespace cell_automaton::rules;

// SIMD headers - only include on x86/x64
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#include <immintrin.h>
#endif

constexpr size_t CHUNK_SIZE = 64;
constexpr double DENSITY_THRESHOLD = 0.3;

template<typename StateT>
struct Coord {
    int32_t x, y;
    bool operator==(const Coord& other) const { return x == other.x && y == other.y; }
};

template<typename StateT>
struct CoordHash {
    size_t operator()(const Coord<StateT>& c) const {
        return std::hash<int64_t>{}((int64_t(c.x) << 32) | int64_t(c.y));
    }
};

template<typename StateT>
struct PairHash {
    size_t operator()(const std::pair<int32_t, int32_t>& p) const {
        return std::hash<int64_t>{}((int64_t(p.first) << 32) | int64_t(p.second));
    }
};

// Hash specialization for ChunkCoord (pair<int32_t, int32_t>)
struct ChunkCoordHash {
    size_t operator()(const std::pair<int32_t, int32_t>& p) const {
        return std::hash<int64_t>{}((int64_t(p.first) << 32) | int64_t(p.second));
    }
};


template<typename StateT>
class Chunk {
private:
    bool is_dense = false;
    std::unordered_map<Coord<StateT>, StateT, CoordHash<StateT>> sparse_data;
    std::array<StateT, CHUNK_SIZE * CHUNK_SIZE> dense_data;
    
public:
    void convert_to_dense();
    void convert_to_sparse();
    StateT get_cell(int x, int y) const;
    void set_cell(int x, int y, StateT state);
    bool is_empty() const;
    bool should_be_dense() const;
    bool should_be_sparse() const;
};

template<typename StateT>
class CellularAutomaton {
private:
    using ChunkCoord = std::pair<int32_t, int32_t>;
    using ChunkMap = std::unordered_map<ChunkCoord, std::unique_ptr<Chunk<StateT>>, ChunkCoordHash>;
    
    ChunkMap chunks;
    std::unique_ptr<Rule<StateT>> rule;
    StateT default_state;
    int64_t generation = 0;
    
    // Chunk coordinate from world coordinate
    ChunkCoord get_chunk_coord(int32_t x, int32_t y) const;
    
    // Local coordinate within chunk
    std::pair<int, int> get_local_coord(int32_t x, int32_t y) const;
    
    Chunk<StateT>* get_or_create_chunk(ChunkCoord coord);
    std::vector<StateT> get_neighbors(int32_t x, int32_t y) const;
    
public:
    CellularAutomaton(std::unique_ptr<Rule<StateT>> r, StateT default_val = StateT{})
        : rule(std::move(r)), default_state(default_val) {}
    
    StateT get_cell(int32_t x, int32_t y) const;
    void set_cell(int32_t x, int32_t y, StateT state);
    void step();
    void run(int64_t iterations);
    
    int64_t get_generation() const { return generation; }
    size_t get_active_chunks() const { return chunks.size(); }
};

template<typename StateT>
void print_pattern(const CellularAutomaton<StateT>& ca, int start_x, int start_y, int width, int height);

#endif // CELLULAR_AUTOMATON_HPP