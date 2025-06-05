cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0079 NEW)

project(gameofme VERSION 0.1.0 LANGUAGES C CXX)

add_subdirectory(rules)
add_subdirectory(cell_automaton)
add_subdirectory(patterns)
add_subdirectory(main)
add_subdirectory(benchmark)

target_link_libraries(gameofme PRIVATE cell_automaton)