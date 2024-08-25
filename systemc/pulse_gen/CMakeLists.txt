cmake_minimum_required(VERSION 3.12.0)
project(demo_bus VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14 CACHE STRING "C++ standard to be used")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(LIBRARY_NAME counter)
set(EXECUTABLE_NAME Executable)

set(SYSTEMC_INC_DIR /Users/thanhduong/Documents/Code_Development/libs/systemc-2.3.3/src)
set(SYSTEMC_LIB_DIR /Users/thanhduong/Documents/Code_Development/libs/systemc-2.3.3/build/src/libsystemc.dylib)


if(${ENABLE_WARNINGS})
    include(Warnings)
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -pg")

set(THREADS_PREFER_PTHREAD_FLAG ON)
# find_package(SystemCLanguage CONFIG REQUIRED)
# find_package(Threads REQUIRED) # find the package thread

set(MY_SRCS
    main.cpp
    pulse_gen.cpp
)

add_executable(${EXECUTABLE_NAME} 
    ${MY_SRCS} 
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC 
    "./"
    ${SYSTEMC_INC_DIR}
)

target_link_libraries(
    ${EXECUTABLE_NAME}
    PUBLIC ${SYSTEMC_LIB_DIR}
)
