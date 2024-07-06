function(tv_message mode)
    if (MASTER_PROJECT)
        set(msg)
    else()
        set(msg "(${PROJECT_NAME}) ")
    endif()
    foreach(i ${ARGN})
        set(msg "${msg}${i}")
    endforeach()
    message(${mode} ${msg})
endfunction()

function(tv_message_mp)
    if (MASTER_PROJECT)
        tv_message(${ARGN})
    endif()
endfunction()

function(tv_enable_unity target)
    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16.0" AND SUPPORTS_COUNTER_MACRO)
        set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
    endif()
endfunction()

function(tv_set_output_dir target)
    # Place everything in the build directory, for ease of use.
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${TV_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${TV_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${TV_BINARY_DIR}
    )
endfunction()

function(tv_add_private_includes target)
    target_include_directories(${target} PRIVATE
        "${PROJECT_SOURCE_DIR}/include/tvision"
        "${PROJECT_SOURCE_DIR}/include/tvision/compat/borland"
    )
    if (NOT WIN32)
        target_include_directories(${target} PRIVATE
            "${PROJECT_SOURCE_DIR}/include/tvision/compat/windows"
        )
    endif()
    if (NOT WIN32 AND NOT (CMAKE_SYSTEM_NAME STREQUAL "Linux"))
        target_include_directories(${target} PRIVATE
            "${PROJECT_SOURCE_DIR}/include/tvision/compat/malloc"
        )
    endif()
endfunction()

function(tv_set_warnings target)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE
            /wd4068 # Unrecognized pragmas
            /wd4146 # Unsigned despite minus sign
            /wd4166 # Illegal calling convention for constructor/destructor
            /wd4244 # Possible data loss in type conversion
            /wd4250 # Inheritance via dominance
            /wd4267 # Possible data loss in conversion from size_t to other type
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wno-deprecated
            -Wno-unknown-pragmas
            -Wno-pragmas
        )
    endif()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target} PRIVATE
            -Wno-overloaded-virtual
        )
    endif()
endfunction()
