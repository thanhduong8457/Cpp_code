if(ENABLE_TESTING)
    set(TEST_MAIN "units_test")
    set(TEST_SOURCE "main.cc")
    set(TEST_INCLUDE "./")

    add_executable(${TEST_MAIN} ${TEST_SOURCE})
    target_include_directories(${TEST_MAIN} PUBLIC {TEST_INCLUDE})
    target_link_libraries(${TEST_MAIN} PUBLIC ${LIBRARY_NAME} Catch2::Catch2)
endif()
