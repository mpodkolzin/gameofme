#include <unordered_map>
#include <vector>
#include <array>
#include <memory>
#include <immintrin.h>  // AVX2 goodness

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
class Chunk {
    bool is_dense = false;
    std::unordered_map<Coord<StateT>, StateT, CoordHash<StateT>> sparse_data;
    std::array<StateT, CHUNK_SIZE * CHUNK_SIZE> dense_data;
    
public:
    void convert_to_dense();
    void convert_to_sparse();
    StateT get_cell(int x, int y) const;
    void set_cell(int x, int y, StateT state);
};