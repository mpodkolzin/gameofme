#ifndef PATTERN_LIBRARY_HPP
#define PATTERN_LIBRARY_HPP

#include "cell_automaton/cellular_automaton.hpp"

namespace cell_automaton {
namespace patterns {

// Pattern creation functions
void create_glider(CellularAutomaton<bool>& ca, int x, int y);
void create_r_pentomino(CellularAutomaton<bool>& ca, int x, int y);
void create_gosper_glider_gun(CellularAutomaton<bool>& ca, int x, int y);
void create_random_soup(CellularAutomaton<bool>& ca, int x, int y, int width, int height, double density = 0.3);

// Utility functions
void print_pattern(const CellularAutomaton<bool>& ca, int min_x, int min_y, int max_x, int max_y);

} // namespace patterns
} // namespace cell_automaton

#endif // PATTERN_LIBRARY_HPP