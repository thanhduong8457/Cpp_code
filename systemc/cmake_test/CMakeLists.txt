cmake_minimum_required(VERSION 3.12.0)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard to be used")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBRARY_NAME Library)
set(EXECUTABLE_NAME Executable)

set(SYSTEMC_INC_DIR /Users/thanhduong/Documents/Code_Development/libs/systemc-2.3.3/src)
set(SYSTEMC_LIB_DIR /Users/thanhduong/Documents/Code_Development/libs/systemc-2.3.3/build/src/libsystemc.dylib)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# include(AddGitSubModule) # add define function
# add_git_submodule(external/systemc-2.3.3)
# include(Docs)

# include(FetchContent)
# FetchContent_Declare(
#     SystemCLanguage 
#     GIT_REPOSITORY https://github.com/yoshijava/systemc-2.3.3.git
#     #GIT_TAG 
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(SystemCLanguage)

add_subdirectory(src)

add_executable(${EXECUTABLE_NAME} main.cpp)
target_include_directories(${EXECUTABLE_NAME} PUBLIC 
    ${SYSTEMC_INC_DIR}
)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    ${LIBRARY_NAME} 
    ${SYSTEMC_LIB_DIR}
    ${PYTHON_LIB_DIR}
)
