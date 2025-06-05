add_executable(gameofme
    main.cpp
)

target_link_libraries(gameofme PRIVATE rules)
target_link_libraries(gameofme PRIVATE cell_automaton)
target_link_libraries(gameofme PRIVATE patterns)
