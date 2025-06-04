add_library(cell_automations STATIC 
    cellular_automations.cpp
    )

target_include_directories(cell_automations PUBLIC ${CMAKE_SOURCE_DIR}/include)
