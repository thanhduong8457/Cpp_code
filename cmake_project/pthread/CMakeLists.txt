cmake_minimum_required(VERSION 3.12.0)
project(demo VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBRARY_NAME Library)
set(EXECUTABLE_NAME Executable)

option(ENABLE_TESTING "Enable a unit test build" ON)

option(ENABLE_WARNINGS "Enable warning" ON)
option(ENABLE_WARNINGS_AS_ERRORS "Enable warning as ctest_empty_binary_directory()" ON)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# include(AddGitSubModule) # add define function
# add_git_submodule(external/systemc-2.3.3)

# include(Docs)

if(${ENABLE_WARNINGS})
    include(Warnings)
endif()

include(FetchContent)
# FetchContent_Declare(
#     SystemCLanguage 
#     GIT_REPOSITORY https://github.com/yoshijava/systemc-2.3.3.git
#     #GIT_TAG 
#     GIT_SHALLOW TRUE
# )
# FetchContent_MakeAvailable(SystemCLanguage)

FetchContent_Declare(
    Catch2 
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v2.13.9
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Catch2)


add_subdirectory(src) # add custom sub directory
add_subdirectory(tests)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED) # find the package thread

add_executable(${EXECUTABLE_NAME} main.cpp)
target_include_directories(${EXECUTABLE_NAME} PUBLIC 
    "src/my_lib" 
    # "external/systemc-2.3.3/src"
)

target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    ${LIBRARY_NAME} 
    Threads::Threads # define library need to link
    # SystemC::systemc
)

if(${ENABLE_WARNINGS})
    target_set_warnings(
        ${EXECUTABLE_NAME}
        ${ENABLE_WARNINGS}
        ${ENABLE_WARNINGS_AS_ERRORS}
    )
endif()
