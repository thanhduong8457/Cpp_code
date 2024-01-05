cmake_minimum_required(VERSION 3.0.0)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODEL_PATH "${PROJECT_SOURCE_DIR}")

# add_subdirectory(PubSubConnector)

set(SOURCES
    main.cpp
)

add_executable(demo ${SOURCES})
