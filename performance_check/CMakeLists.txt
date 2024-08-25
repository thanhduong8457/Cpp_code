cmake_minimum_required(VERSION 3.10)
project(PerformanceTest)
set(CMAKE_CXX_STANDARD 14)

# Add profiling flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")

# Add the path to the gperftools library

add_executable(performance_test main.cpp)
