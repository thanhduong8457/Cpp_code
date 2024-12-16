cmake_minimum_required(VERSION 3.12.0)
project(ut_top VERSION 0.1.0 LANGUAGES C CXX)

set(EXECUTABLE_NAME ut_top)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find OpenSSL
# find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(${EXECUTABLE_NAME} main.cpp delta_robot.cpp codos.cpp)

# Link OpenSSL libraries
# target_link_libraries(${EXECUTABLE_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Include directories for the project and OpenSSL
target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/
)
