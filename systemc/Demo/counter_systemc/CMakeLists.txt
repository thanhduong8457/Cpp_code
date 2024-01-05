cmake_minimum_required(VERSION 3.12)
project(counter VERSION 0.1.0 LANGUAGES C CXX)

message(STATUS "Add subdirectory systemc")
add_subdirectory(
    "${SILKIT_THIRD_PARTY_SOURCE_DIR}/googletest"
    "${SILKIT_THIRD_PARTY_BINARY_DIR}/_tp_googletest"
    EXCLUDE_FROM_ALL
)

# Find the SystemC package
find_package(SystemC REQUIRED)

# Add the include directories for SystemC
# include_directories(${SystemC_INCLUDE_DIRS})

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 14)

include(CTest)
# enable_testing()

add_executable(counter main.cpp counter.cpp counter_test.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Link against the SystemC library
target_link_libraries(your_executable_name ${SystemC_LIBRARIES})
