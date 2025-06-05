#include "rules/conway_rule.hpp"

namespace cell_automaton {
namespace rules {

bool ConwayRule::apply(bool current, const std::vector<bool>& neighbors) const {
    // Count live neighbors
    int live_neighbors = 0;
    for (bool neighbor : neighbors) {
        if (neighbor) {
            live_neighbors++;
        }
    }
    
    if (current) {
        // Live cell: survives with 2 or 3 neighbors
        return live_neighbors == 2 || live_neighbors == 3;
    } else {
        // Dead cell: born with exactly 3 neighbors
        return live_neighbors == 3;
    }
}

std::unique_ptr<Rule<bool>> ConwayRule::clone() const {
    return std::make_unique<ConwayRule>(*this);
}

} // namespace rules
} // namespace cell_automaton