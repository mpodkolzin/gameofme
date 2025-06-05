#include "cell_automaton/cellular_automaton.hpp"
#include <algorithm>
// #include <execution>  // Not available on all platforms

// ============================================================================
// Chunk Implementation
// ============================================================================

template<typename StateT>
void Chunk<StateT>::convert_to_dense() {
    if (is_dense) return;
    
    // Initialize dense array with default values
    dense_data.fill(StateT{});
    
    // Copy sparse data to dense
    for (const auto& [coord, state] : sparse_data) {
        if (coord.x >= 0 && coord.x < CHUNK_SIZE && coord.y >= 0 && coord.y < CHUNK_SIZE) {
            dense_data[coord.y * CHUNK_SIZE + coord.x] = state;
        }
    }
    
    sparse_data.clear();
    is_dense = true;
}

template<typename StateT>
void Chunk<StateT>::convert_to_sparse() {
    if (!is_dense) return;
    
    sparse_data.clear();
    
    // Only store non-default states
    for (int y = 0; y < CHUNK_SIZE; ++y) {
        for (int x = 0; x < CHUNK_SIZE; ++x) {
            StateT state = dense_data[y * CHUNK_SIZE + x];
            if (state != StateT{}) {
                sparse_data[{x, y}] = state;
            }
        }
    }
    
    is_dense = false;
}

template<typename StateT>
StateT Chunk<StateT>::get_cell(int x, int y) const {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
        return StateT{};  // Out of bounds
    }
    
    if (is_dense) {
        return dense_data[y * CHUNK_SIZE + x];
    } else {
        auto it = sparse_data.find({x, y});
        return (it != sparse_data.end()) ? it->second : StateT{};
    }
}

template<typename StateT>
void Chunk<StateT>::set_cell(int x, int y, StateT state) {
    if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE) {
        return;  // Out of bounds
    }
    
    if (is_dense) {
        dense_data[y * CHUNK_SIZE + x] = state;
        
        // Consider converting to sparse if density drops
        if (should_be_sparse()) {
            convert_to_sparse();
        }
    } else {
        if (state != StateT{}) {
            sparse_data[{x, y}] = state;
        } else {
            sparse_data.erase({x, y});
        }
        
        // Consider converting to dense if density increases
        if (should_be_dense()) {
            convert_to_dense();
        }
    }
}

template<typename StateT>
bool Chunk<StateT>::is_empty() const {
    if (is_dense) {
        return std::all_of(dense_data.begin(), dense_data.end(), 
                          [](const StateT& s) { return s == StateT{}; });
    } else {
        return sparse_data.empty();
    }
}

template<typename StateT>
bool Chunk<StateT>::should_be_dense() const {
    if (is_dense) return true;
    
    size_t non_default_count = sparse_data.size();
    double density = static_cast<double>(non_default_count) / (CHUNK_SIZE * CHUNK_SIZE);
    return density > DENSITY_THRESHOLD;
}

template<typename StateT>
bool Chunk<StateT>::should_be_sparse() const {
    if (!is_dense) return true;
    
    size_t non_default_count = std::count_if(dense_data.begin(), dense_data.end(),
                                            [](const StateT& s) { return s != StateT{}; });
    double density = static_cast<double>(non_default_count) / (CHUNK_SIZE * CHUNK_SIZE);
    return density <= DENSITY_THRESHOLD * 0.5;  // Hysteresis to prevent thrashing
}

// ============================================================================
// CellularAutomaton Implementation
// ============================================================================

template<typename StateT>
typename CellularAutomaton<StateT>::ChunkCoord 
CellularAutomaton<StateT>::get_chunk_coord(int32_t x, int32_t y) const {
    return {x / CHUNK_SIZE - (x < 0 ? 1 : 0), 
            y / CHUNK_SIZE - (y < 0 ? 1 : 0)};
}

template<typename StateT>
std::pair<int, int> CellularAutomaton<StateT>::get_local_coord(int32_t x, int32_t y) const {
    int lx = x % CHUNK_SIZE;
    int ly = y % CHUNK_SIZE;
    if (lx < 0) lx += CHUNK_SIZE;
    if (ly < 0) ly += CHUNK_SIZE;
    return {lx, ly};
}

template<typename StateT>
Chunk<StateT>* CellularAutomaton<StateT>::get_or_create_chunk(ChunkCoord coord) {
    auto it = chunks.find(coord);
    if (it != chunks.end()) {
        return it->second.get();
    }
    
    auto new_chunk = std::make_unique<Chunk<StateT>>();
    auto* ptr = new_chunk.get();
    chunks[coord] = std::move(new_chunk);
    return ptr;
}

template<typename StateT>
StateT CellularAutomaton<StateT>::get_cell(int32_t x, int32_t y) const {
    ChunkCoord chunk_coord = get_chunk_coord(x, y);
    auto it = chunks.find(chunk_coord);
    
    if (it == chunks.end()) {
        return default_state;
    }
    
    auto [lx, ly] = get_local_coord(x, y);
    return it->second->get_cell(lx, ly);
}

template<typename StateT>
void CellularAutomaton<StateT>::set_cell(int32_t x, int32_t y, StateT state) {
    if (state == default_state) {
        // Setting to default - only need to clear if chunk exists
        ChunkCoord chunk_coord = get_chunk_coord(x, y);
        auto it = chunks.find(chunk_coord);
        if (it != chunks.end()) {
            auto [lx, ly] = get_local_coord(x, y);
            it->second->set_cell(lx, ly, state);
        }
    } else {
        // Setting to non-default - create chunk if needed
        ChunkCoord chunk_coord = get_chunk_coord(x, y);
        Chunk<StateT>* chunk = get_or_create_chunk(chunk_coord);
        auto [lx, ly] = get_local_coord(x, y);
        chunk->set_cell(lx, ly, state);
    }
}

template<typename StateT>
std::vector<StateT> CellularAutomaton<StateT>::get_neighbors(int32_t x, int32_t y) const {
    std::vector<StateT> neighbors;
    neighbors.reserve(8);  // Moore neighborhood
    
    // 8-connected neighborhood
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;  // Skip center cell
            neighbors.push_back(get_cell(x + dx, y + dy));
        }
    }
    
    return neighbors;
}

template<typename StateT>
void CellularAutomaton<StateT>::step() {
    // Collect all cells that need evaluation (active cells + their neighbors)
    std::unordered_set<std::pair<int32_t, int32_t>, PairHash<StateT>> cells_to_evaluate;
    
    // Gather all active cells and their neighborhoods
    for (const auto& [chunk_coord, chunk_ptr] : chunks) {
        int32_t chunk_x = chunk_coord.first * CHUNK_SIZE;
        int32_t chunk_y = chunk_coord.second * CHUNK_SIZE;
        
        for (int ly = 0; ly < CHUNK_SIZE; ++ly) {
            for (int lx = 0; lx < CHUNK_SIZE; ++lx) {
                StateT state = chunk_ptr->get_cell(lx, ly);
                if (state != default_state) {
                    int32_t world_x = chunk_x + lx;
                    int32_t world_y = chunk_y + ly;
                    
                    // Add this cell and all its neighbors to evaluation set
                    for (int dy = -1; dy <= 1; ++dy) {
                        for (int dx = -1; dx <= 1; ++dx) {
                            cells_to_evaluate.insert({world_x + dx, world_y + dy});
                        }
                    }
                }
            }
        }
    }
    
    // Calculate new states (double buffering approach)
    std::vector<std::tuple<int32_t, int32_t, StateT>> updates;
    updates.reserve(cells_to_evaluate.size());
    
    // Parallel evaluation of cell updates (fallback to sequential if no execution policy)
    std::vector<std::pair<int32_t, int32_t>> eval_cells(cells_to_evaluate.begin(), cells_to_evaluate.end());
    std::vector<std::tuple<int32_t, int32_t, StateT>> temp_updates(eval_cells.size());
    
    std::transform(eval_cells.begin(), eval_cells.end(), temp_updates.begin(),
                   [this](const auto& coord) {
                       auto [x, y] = coord;
                       StateT current = get_cell(x, y);
                       std::vector<StateT> neighbors = get_neighbors(x, y);
                       StateT new_state = rule->apply(current, neighbors);
                       return std::make_tuple(x, y, new_state);
                   });
    
    // Filter out unchanged cells
    for (const auto& [x, y, new_state] : temp_updates) {
        StateT current = get_cell(x, y);
        if (new_state != current) {
            updates.emplace_back(x, y, new_state);
        }
    }
    
    // Apply all updates
    for (const auto& [x, y, new_state] : updates) {
        set_cell(x, y, new_state);
    }
    
    // Cleanup empty chunks
    auto it = chunks.begin();
    while (it != chunks.end()) {
        if (it->second->is_empty()) {
            it = chunks.erase(it);
        } else {
            ++it;
        }
    }
    
    ++generation;
}

template<typename StateT>
void CellularAutomaton<StateT>::run(int64_t iterations) {
    for (int64_t i = 0; i < iterations; ++i) {
        step();
    }
}

template<typename StateT>
void print_pattern(const CellularAutomaton<StateT>& ca, int start_x, int start_y, int width, int height) {
    for (int y = start_y; y < start_y + height; ++y) {
        for (int x = start_x; x < start_x + width; ++x) {
            std::cout << (ca.get_cell(x, y) ? '#' : '.');
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

// Explicit template instantiations for common types
template class Chunk<bool>;
template class Chunk<int>;
template class Chunk<uint8_t>;
template class CellularAutomaton<bool>;
template class CellularAutomaton<int>;
template class CellularAutomaton<uint8_t>;

// Explicit template instantiations for print_pattern function
template void print_pattern(const CellularAutomaton<bool>& ca, int start_x, int start_y, int width, int height);
template void print_pattern(const CellularAutomaton<uint8_t>& ca, int start_x, int start_y, int width, int height);