add_executable(benchmark_runner
    benchmark_runner.cpp
)

target_link_libraries(benchmark_runner PRIVATE rules)
target_link_libraries(benchmark_runner PRIVATE cell_automaton)
target_link_libraries(benchmark_runner PRIVATE patterns)