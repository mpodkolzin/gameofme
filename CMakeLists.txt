cmake_minimum_required(VERSION 3.10.0)
project(gameofme VERSION 0.1.0 LANGUAGES C CXX)

add_executable(gameofme main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


# ----------------------
# Your Code
# ----------------------
add_subdirectory(src)
#add_subdirectory(tests)
