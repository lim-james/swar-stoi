cmake_minimum_required(VERSION 3.23)
project(GEMM LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-O3 -march=native)

add_library(swar_stoi INTERFACE)
target_include_directories(swar_stoi INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---------- TESTS ----------
enable_testing()

add_executable(swar_correctness tests/validate_correctness.cpp)
target_link_libraries(swar_correctness PRIVATE swar_stoi)
add_test(NAME SWAR.Correctness COMMAND swar_correctness)


