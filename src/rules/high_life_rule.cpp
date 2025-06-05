#include "rules/rule_base.hpp"
#include <iostream>
#include <chrono>

namespace cell_automaton {
namespace rules {

// ============================================================================
// HighLife (B36/S23) - has interesting replicators
// ============================================================================
class HighLifeRule : public Rule<bool> {
public:
    bool apply(bool current, const std::vector<bool>& neighbors) const override {
        int live_neighbors = 0;
        for (bool neighbor : neighbors) {
            if (neighbor) live_neighbors++;
        }
        
        if (current) {
            // Live cell: survives with 2 or 3 neighbors
            return live_neighbors == 2 || live_neighbors == 3;
        } else {
            // Dead cell: born with 3 or 6 neighbors
            return live_neighbors == 3 || live_neighbors == 6;
        }
    }
    
    std::unique_ptr<Rule<bool>> clone() const override {
        return std::make_unique<HighLifeRule>(*this);
    }
};

} // namespace rules
} // namespace cell_automaton