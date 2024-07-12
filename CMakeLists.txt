cmake_minimum_required(VERSION 2.8...3.14)

project(
    c89atomic
    VERSION 0.0.0
    DESCRIPTION "C89 compatible atomics."
    HOMEPAGE_URL "https://github.com/mackron/c89atomic"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 90)

include(CMakeDependentOption)
include(GNUInstallDirs)
message("Generated with config types: ${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_CONFIGURATION_TYPES=Debug;Release)
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/built")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

enable_testing()
add_subdirectory(tests)
