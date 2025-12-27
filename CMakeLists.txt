cmake_minimum_required(VERSION 3.23)
project(SwarStoi LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(swar_stoi_lib INTERFACE)

target_include_directories(swar_stoi_lib INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_compile_options(-O3 -march=native -Wall -Wextra -Wpedantic)

add_subdirectory(apps)

include(CTest)
enable_testing()
add_subdirectory(tests)
