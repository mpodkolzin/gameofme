// ============================================================================
// Pattern creation helpers - these were missing from the implementation
// ============================================================================

#include "cell_automaton/cellular_automaton.hpp"
#include <iostream>
#include <cstdlib>


namespace cell_automaton {
namespace patterns {

/**
 * Create a glider pattern starting at (x, y)
 * Classic small spaceship that moves diagonally
 */
void create_glider(CellularAutomaton<bool>& ca, int x, int y) {
    ca.set_cell(x+1, y, true);
    ca.set_cell(x+2, y+1, true);
    ca.set_cell(x, y+2, true);
    ca.set_cell(x+1, y+2, true);
    ca.set_cell(x+2, y+2, true);
}

/**
 * Create an R-pentomino pattern starting at (x, y)
 * Famous methuselah that stabilizes after 1103 generations
 */
void create_r_pentomino(CellularAutomaton<bool>& ca, int x, int y) {
    ca.set_cell(x+1, y, true);
    ca.set_cell(x+2, y, true);
    ca.set_cell(x, y+1, true);
    ca.set_cell(x+1, y+1, true);
    ca.set_cell(x+1, y+2, true);
}

/**
 * Create a random soup of cells
 * @param ca The cellular automaton
 * @param x Starting x coordinate
 * @param y Starting y coordinate
 * @param width Width of the soup area
 * @param height Height of the soup area
 * @param density Probability of each cell being alive (0.0 to 1.0)
 */
void create_random_soup(CellularAutomaton<bool>& ca, int x, int y, int width, int height, double density = 0.3) {
    for (int dy = 0; dy < height; ++dy) {
        for (int dx = 0; dx < width; ++dx) {
            if (static_cast<double>(rand()) / RAND_MAX < density) {
                ca.set_cell(x + dx, y + dy, true);
            }
        }
    }
}

/**
 * Print a rectangular region of the cellular automaton
 * @param ca The cellular automaton to print
 * @param min_x Left boundary
 * @param min_y Top boundary  
 * @param max_x Right boundary
 * @param max_y Bottom boundary
 */
void print_pattern(const CellularAutomaton<bool>& ca, int min_x, int min_y, int max_x, int max_y) {
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            std::cout << (ca.get_cell(x, y) ? '#' : '.');
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

/**
 * Create a Gosper glider gun pattern
 * Classic pattern that generates gliders indefinitely
 */
void create_gosper_glider_gun(CellularAutomaton<bool>& ca, int x, int y) {
    // Left square
    ca.set_cell(x+1, y+5, true);
    ca.set_cell(x+1, y+6, true);
    ca.set_cell(x+2, y+5, true);
    ca.set_cell(x+2, y+6, true);
    
    // Left part of gun
    ca.set_cell(x+11, y+5, true);
    ca.set_cell(x+11, y+6, true);
    ca.set_cell(x+11, y+7, true);
    ca.set_cell(x+12, y+4, true);
    ca.set_cell(x+12, y+8, true);
    ca.set_cell(x+13, y+3, true);
    ca.set_cell(x+13, y+9, true);
    ca.set_cell(x+14, y+3, true);
    ca.set_cell(x+14, y+9, true);
    ca.set_cell(x+15, y+6, true);
    ca.set_cell(x+16, y+4, true);
    ca.set_cell(x+16, y+8, true);
    ca.set_cell(x+17, y+5, true);
    ca.set_cell(x+17, y+6, true);
    ca.set_cell(x+17, y+7, true);
    ca.set_cell(x+18, y+6, true);
    
    // Right part of gun
    ca.set_cell(x+21, y+3, true);
    ca.set_cell(x+21, y+4, true);
    ca.set_cell(x+21, y+5, true);
    ca.set_cell(x+22, y+3, true);
    ca.set_cell(x+22, y+4, true);
    ca.set_cell(x+22, y+5, true);
    ca.set_cell(x+23, y+2, true);
    ca.set_cell(x+23, y+6, true);
    ca.set_cell(x+25, y+1, true);
    ca.set_cell(x+25, y+2, true);
    ca.set_cell(x+25, y+6, true);
    ca.set_cell(x+25, y+7, true);
    
    // Right square
    ca.set_cell(x+35, y+3, true);
    ca.set_cell(x+35, y+4, true);
    ca.set_cell(x+36, y+3, true);
    ca.set_cell(x+36, y+4, true);
}

} // namespace patterns
} // namespace cell_automaton