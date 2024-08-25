set(LIBRARY_SOURCES
    "design.cpp"
)

set(LIBRARY_HEADERS
    "design.h"
)

set(LIBRARY_INCLUDES
    "./"
    # "$(CMAKE_BINARY_DIR)/configure_files/include"
    # "$(CMAKE_BINARY_DIR)/external/systemc-2.3.3/src"
)

# find_package(SystemCLanguage CONFIG REQUIRED)

add_library(${LIBRARY_NAME} STATIC
    ${LIBRARY_SOURCES}
    # ${LIBRARY_HEADERS}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
    ${LIBRARY_INCLUDES}
    ${SYSTEMC_INC_DIR}
)

target_link_libraries( ${LIBRARY_NAME} PUBLIC
    # SystemC::systemc
    ${SYSTEMC_LIB_DIR}
)