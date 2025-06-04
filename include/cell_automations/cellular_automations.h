template<typename StateT, typename RuleT, size_t ChunkSize = 64>
class CellularAutomaton {
    using StateMap = std::unordered_map<int64_t, StateT>;
    using ChunkID = std::pair<int32_t, int32_t>;
    
    struct Chunk {
        bool is_dense;
        union {
            StateMap sparse;
            std::array<StateT, ChunkSize * ChunkSize> dense;
        };
    };
};