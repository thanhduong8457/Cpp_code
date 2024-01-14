function(add_sanitixer_flags) # sanitizer for detecring memory leak problem during runtime, check memory issue
    if (NOT ${ENABLE_ADDRESS_SANITIZER} AND NOT ${ENABLE_UNDEFINED_SANITIZER})
        message(STATUS "Sanitizer deactivate.")
        return()
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options("-fno-omit-frame-pointer")
        add_link_options("-fno-omit-frame-pointer")

        if (${ENABLE_ADDRESS_SANITIZER})
            add_compile_options("-fsanitize=address")
            add_link_options("-fsanitize=address")
        endif()

        if (${ENABLE_ADDRESS_SANITIZER})
            add_compile_options("-fsanitize=undefined")
            add_link_options("-fsanitize=undefined")
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if (${ENABLE_ADDRESS_SANITIZER})
            add_compile_options("-/fsanitize=address")
        endif()

        if (${ENABLE_ADDRESS_SANITIZER})
            message(STATUS "Undifined sanitizer, not implement for MSVC")
        endif()
    else()
        message(ERROR "Compiler not supported for Sanitizers.")
    endif()
endfunction()
