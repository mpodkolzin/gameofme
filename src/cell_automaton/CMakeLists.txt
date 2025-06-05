add_library(cell_automaton STATIC 
    cellular_automaton.cpp
    )

target_include_directories(cell_automaton PUBLIC ${CMAKE_SOURCE_DIR}/include)
