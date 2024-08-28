set(LIBRARY_NAME design)

file(GLOB_RECURSE LIBRARY_SOURCES FOLLOW_SYMLINKS
    *.cpp
)

set(LIBRARY_INCLUDES
    ./
)

add_library(${LIBRARY_NAME} STATIC
    ${LIBRARY_SOURCES}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${LIBRARY_INCLUDES}
    ${SYSTEMC_INC_DIR}
)

target_link_libraries( ${LIBRARY_NAME} PUBLIC
    ${SYSTEMC_LIB_DIR}
)
