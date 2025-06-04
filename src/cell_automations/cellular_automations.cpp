
#include "cell_automations/cellular_automations.h"


template<typename StateT>
class CellularAutomaton {
private:
    using ChunkCoord = std::pair<int32_t, int32_t>;
    using ChunkMap = std::unordered_map<ChunkCoord, std::unique_ptr<Chunk<StateT>>>;
    
    ChunkMap chunks;
    std::unique_ptr<Rule<StateT>> rule;
    StateT default_state;
    int64_t generation = 0;
    
    // Chunk coordinate from world coordinate
    ChunkCoord get_chunk_coord(int32_t x, int32_t y) const {
        return {x / CHUNK_SIZE - (x < 0 ? 1 : 0), 
                y / CHUNK_SIZE - (y < 0 ? 1 : 0)};
    }
    
    // Local coordinate within chunk
    std::pair<int, int> get_local_coord(int32_t x, int32_t y) const {
        int lx = x % CHUNK_SIZE;
        int ly = y % CHUNK_SIZE;
        if (lx < 0) lx += CHUNK_SIZE;
        if (ly < 0) ly += CHUNK_SIZE;
        return {lx, ly};
    }
    
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