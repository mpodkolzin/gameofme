#include <iostream>
#include "cell_automaton/cellular_automaton.hpp"
#include "cell_automaton/cellular_automaton.hpp"
#include "rules/conway_rule.hpp"
#include "patterns/patterns_library.hpp"

using namespace cell_automaton;
using namespace cell_automaton::rules;
using namespace cell_automaton::patterns;

// ============================================================================
// Test harness - extracted from the main() function
// ============================================================================

int main() {
    std::cout << "=== Cellular Automaton Test Suite ===\n\n";
    
    // Test 1: Conway's Game of Life with R-pentomino
    {
        std::cout << "Test 1: Conway's Game of Life - R-pentomino\n";
        auto rule = std::make_unique<ConwayRule>();
        CellularAutomaton<bool> ca(std::move(rule), false);
        
        create_r_pentomino(ca, 10, 10);
        
        std::cout << "Initial state:\n";
        print_pattern(ca, 8, 8, 14, 14);
        
        std::cout << "After 10 generations:\n";
        ca.run(10);
        print_pattern(ca, 5, 5, 20, 20);
        
        std::cout << "Generation: " << ca.get_generation() << std::endl;
        std::cout << "Active chunks: " << ca.get_active_chunks() << "\n\n";
    }
    
    
    std::cout << "\n=== All tests completed! ===\n";
    return 0;
}