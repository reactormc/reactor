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

# Get zlib
find_package(ZLIB)

# Get libtomcrypt
find_library(TOMCRYPT_LIB tomcrypt)
if (NOT TOMCRYPT_LIB)
    message(FATAL_ERROR "libtomcrypt not found")
endif()

# Get libtommath
find_library(TOMMATH_LIB tommath)
if (NOT TOMMATH_LIB)
    message(FATAL_ERROR "libtommath not found")
endif()

# Get libcurl
find_package(CURL)
if (NOT CURL_FOUND)
    message(FATAL_ERROR "libcurl not found")
endif()

# All sources
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ./*.c ./*.h)

# Set libraries
set(REACTOR_MODULES REACTOR_NET REACTOR_UTIL REACTOR_WORLD)
set(REACTOR_LIBS ${REACTOR_MODULES} ${UNISTRING_LIB} ${JSONC_LIB} ZLIB::ZLIB ${TOMCRYPT_LIB} ${TOMMATH_LIB} ${CURL_LIBRARIES})

# Release build
add_executable(reactor ${SOURCES})
target_link_libraries(reactor ${REACTOR_LIBS})

# Debug build
add_executable(reactor_debug ${SOURCES})
target_compile_definitions(reactor_debug PUBLIC DEBUG)
target_link_libraries(reactor_debug ${REACTOR_LIBS})