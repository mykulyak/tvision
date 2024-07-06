# Options summary:
#
# * TV_BUILD_EXAMPLES (default ON) to build example applications.
# * TV_BUILD_USING_GPM (default ON) (only on linux) to enable linking to libgpm
#   (warning if not found).
# * TV_USE_STATIC_RTL (default OFF) to link against the static version of the
#   runtime library (MSVC only).
# * TV_REDUCE_APP_SIZE (default OFF) to reduce executable size by asking the
#   linker to strip out unused funcions and data. This affects the library's PUBLIC
#   linker flags.
# * TV_OPTIMIZE_BUILD (default ON) to build with Precompiled Headers (CMake 3.16
#   or newer).
# * TV_LIBRARY_UNITY_BUILD (default OFF) to reduce the build time of the main library
#   (CMake 3.16 or newer), at the cost of possibly increasing its size.
# * TV_BUILD_AVSCOLOR (default OFF) to build an AviSynth plugin for testing the
#   terminal color quantization (Unix only).
# * TV_BUILD_TESTS (default OFF) to build and run tests.

# TODO:
#
# * TV_CLANG_ASAN (default OFF) to enable AddressSanitizer.
# * TV_CLANG_FORMAT (default OFF) to run clang-format on the source code.
# * TV_CLANG_TIDY (default OFF) to run clang-tidy on the source code.

cmake_minimum_required (VERSION 3.24)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(MiscUtils)

cmake_policy(SET CMP0077 NEW) # 'option()' honors normal variables.

set(MASTER_PROJECT FALSE)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(MASTER_PROJECT TRUE)
endif()

project(tvision)

# Project options

set(TV_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    option(TV_BUILD_USING_GPM "Use GPM" ON)
    set(MAY_BUILD_USING_GPM TRUE)
endif()

if (MASTER_PROJECT)
    option(TV_BUILD_EXAMPLES "Build example apps" ON)
endif()

option(TV_USE_STATIC_RTL "Link against the static version of the runtime library (MSVC only)" OFF)
option(TV_REDUCE_APP_SIZE "Strip out unused funcions and data from executables" OFF)
option(TV_LIBRARY_UNITY_BUILD "Use CMake's Unity Build" OFF)
option(TV_OPTIMIZE_BUILD "Use CMake's Precompiled Headers" ON)
option(TV_BUILD_AVSCOLOR "Build AviSynth TermColor plugin" OFF)
option(TV_BUILD_TESTS "Build and run tests" OFF)

if (MSVC)
    set(MAY_USE_STATIC_RTL TRUE)
elseif (TV_USE_STATIC_RTL)
    tv_message(WARNING "'TV_USE_STATIC_RTL' requested but only available in MSVC or equivalent.")
    set(TV_USE_STATIC_RTL OFF)
endif()
if (MAY_USE_STATIC_RTL)
    tv_message_mp(STATUS "Link w/static RTL: ${TV_USE_STATIC_RTL}")
endif()

tv_message_mp(STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")
tv_message(STATUS "Build Examples: ${TV_BUILD_EXAMPLES}")
if (MAY_BUILD_USING_GPM)
    tv_message(STATUS "Build w/GPM: ${TV_BUILD_USING_GPM}")
endif()

add_subdirectory(source)

if (TV_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (TV_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

add_subdirectory(tools)
