cmake_minimum_required(VERSION 3.2)

project(Reactor 
    VERSION 1.0.0.0
    DESCRIPTION "A minecraft server in C"
    LANGUAGES C)

# Get libunistring
find_library(UNISTRING_LIB unistring)
if (NOT UNISTRING_LIB)
    message(FATAL_ERROR "libunistring not found")
endif()

# Get json-c
find_library(JSONC_LIB json-c)
if (NOT JSONC_LIB)
    message(FATAL_ERROR "json-c not found")
endif()

# All sources
file(GLOB_RECURSE SOURCES src/*.c src/*.h)

# Set libraries
set(REACTOR_LIBS ${UNISTRING_LIB} ${JSONC_LIB})

# Root folder should be in include path
include_directories(. ./src)

# Check debug
if (DEBUG)
    add_executable(reactor_debug ${SOURCES})
    target_compile_definitions(reactor_debug PRIVATE DEBUG)
    target_link_libraries(reactor_debug ${REACTOR_LIBS})
else()
    add_executable(reactor ${SOURCES})
    target_link_libraries(reactor ${REACTOR_LIBS})
endif()
