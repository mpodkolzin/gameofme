add_library(rules STATIC 
    conway_rule.cpp
    high_life_rule.cpp
)

target_include_directories(rules PUBLIC ${CMAKE_SOURCE_DIR}/include)