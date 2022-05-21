cmake_minimum_required(VERSION 3.1)

project(SofaDesigner LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")

# Put any binary output to build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(extern)
add_subdirectory(libs)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(bin)
