#ifndef RULE_BASE_HPP
#define RULE_BASE_HPP

#include <vector>
#include <memory>

namespace cell_automaton {
namespace rules {

/**
 * Base class for all cellular automaton rules.
 * 
 * @tparam StateT The type representing cell state (bool, int, uint8_t, etc.)
 */
template<typename StateT>
class Rule {
public:
    virtual ~Rule() = default;
    
    /**
     * Apply the rule to a cell given its current state and neighbors.
     * 
     * @param current Current state of the cell
     * @param neighbors Vector of neighbor states (typically 8 for Moore neighborhood)
     * @return New state for the cell
     */
    virtual StateT apply(StateT current, const std::vector<StateT>& neighbors) const = 0;
    
    /**
     * Create a copy of this rule. Useful for threading or storing rule configurations.
     * 
     * @return Unique pointer to a clone of this rule
     */
    virtual std::unique_ptr<Rule<StateT>> clone() const = 0;
    
    /**
     * Get a human-readable name for this rule (optional override)
     */
    virtual const char* name() const { return "Unknown Rule"; }
    
    /**
     * Get rule notation if applicable (e.g., "B3/S23" for Conway)
     */
    virtual const char* notation() const { return ""; }
};

} // namespace rules
} // namespace cell_automaton

#endif // RULE_BASE_HPP