cmake_minimum_required(VERSION 3.14)
project(c89atomic VERSION 1.0)

# Options
option(C89ATOMIC_BUILD_EXAMPLES "Build c89atomic examples" OFF)
option(C89ATOMIC_BUILD_TESTS    "Build c89atomic tests"    OFF)
option(C89ATOMIC_BUILD_TOOLS    "Build c89atomic tools"    OFF)
option(C89ATOMIC_FORCE_CXX      "Force compilation as C++" OFF)
option(C89ATOMIC_FORCE_C89      "Force compilation as C89" OFF)

# Construct compiler options.
set(COMPILE_OPTIONS)

if(C89ATOMIC_FORCE_CXX AND C89ATOMIC_FORCE_C89)
    message(FATAL_ERROR "C89ATOMIC_FORCE_CXX and C89ATOMIC_FORCE_C89 cannot be enabled at the same time.")
endif()

if(C89ATOMIC_FORCE_CXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C++ (GNU/Clang)")
        list(APPEND COMPILE_OPTIONS -x c++)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS "Compiling as C++ (MSVC)")
        list(APPEND COMPILE_OPTIONS /TP)
    else()
        message(WARNING "C89ATOMIC_FORCE_CXX is enabled but the compiler does not support it. Ignoring.")
    endif()
endif()

if(C89ATOMIC_FORCE_C89)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(STATUS "Compiling as C89")
        list(APPEND COMPILE_OPTIONS -std=c89)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(WARNING "MSVC does not support forcing C89. C89ATOMIC_FORCE_C89 ignored.")
    else()
        message(WARNING "C89ATOMIC_FORCE_C89 is enabled but the compiler does not support it. Ignoring.")
    endif()
endif()

# Warnings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND COMPILE_OPTIONS -Wall -Wextra -pedantic)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    #list(APPEND COMPILE_OPTIONS /W4)
endif()


# Construct compiler defines
set(COMPILE_DEFINES)


# Link libraries
set(COMMON_LINK_LIBRARIES)

if (UNIX)
    find_library(LIB_ATOMIC NAMES atomic)
    if(LIB_ATOMIC)
        list(APPEND COMMON_LINK_LIBRARIES ${LIB_ATOMIC})
    endif()
endif()


# Common interface
add_library(c89atomic_common INTERFACE)
target_compile_options    (c89atomic_common INTERFACE ${COMPILE_OPTIONS})
target_link_libraries     (c89atomic_common INTERFACE ${COMMON_LINK_LIBRARIES})
target_compile_definitions(c89atomic_common INTERFACE ${COMPILE_DEFINES})


# Main library
add_library(c89atomic STATIC
    c89atomic.c
    c89atomic.h
)

target_include_directories(c89atomic PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries     (c89atomic PRIVATE c89atomic_common)


# Tests
if(C89ATOMIC_BUILD_TESTS)
    enable_testing()

    add_executable(c89atomic_basic tests/c89atomic_basic.c)
    target_link_libraries(c89atomic_basic PRIVATE c89atomic)
    add_test(NAME c89atomic_basic COMMAND c89atomic_basic)

    # sandbox. Don't add a test for this.
    #add_executable(c89atomic_sandbox tests/c89atomic_sandbox.c)
    #target_link_libraries(c89atomic_sandbox PRIVATE c89atomic)
endif()
