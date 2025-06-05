#ifndef CONWAY_RULE_HPP
#define CONWAY_RULE_HPP

#include "rules/rule_base.hpp"

namespace cell_automaton {
namespace rules {

/**
 * Conway's Game of Life rule (B3/S23)
 * 
 * Classic cellular automaton with simple rules:
 * - Live cell with 2-3 neighbors survives
 * - Dead cell with exactly 3 neighbors becomes alive
 * - All other cells die or remain dead
 */
class ConwayRule : public Rule<bool> {
public:
    ConwayRule() = default;
    
    bool apply(bool current, const std::vector<bool>& neighbors) const override;
    
    std::unique_ptr<Rule<bool>> clone() const override;
    
    const char* name() const override { return "Conway's Game of Life"; }
    
    const char* notation() const override { return "B3/S23"; }
};

} // namespace rules
} // namespace cell_automaton

#endif // CONWAY_RULE_HPP