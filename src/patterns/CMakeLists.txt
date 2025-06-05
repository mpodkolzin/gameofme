add_library(patterns STATIC 
    patterns_library.cpp
    )

target_include_directories(patterns PUBLIC ${CMAKE_SOURCE_DIR}/include)
