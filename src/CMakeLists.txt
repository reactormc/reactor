find_library(UNISTRING_LIB unistring REQUIRED)
find_library(JSONC_LIB json-c REQUIRED)
find_library(TOMCRYPT_LIB tomcrypt REQUIRED)
find_library(TOMMATH_LIB tommath REQUIRED)
find_package(ZLIB REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(pkgconfig_deps REQUIRED IMPORTED_TARGET glib-2.0)

# All sources
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false ./*.c ./*.h)

# Set libraries
set(REACTOR_MODULES REACTOR_NET REACTOR_UTIL REACTOR_WORLD)
set(REACTOR_LIBS
        ${REACTOR_MODULES} ${UNISTRING_LIB} ${JSONC_LIB} ${TOMCRYPT_LIB} ${TOMMATH_LIB}
        ZLIB::ZLIB ${CURL_LIBRARIES} PkgConfig::pkgconfig_deps)

# Release build
add_executable(reactor ${SOURCES})
target_compile_definitions(reactor PUBLIC LTM_DESC USE_LTM)
target_link_libraries(reactor ${REACTOR_LIBS})

# Debug build
add_executable(reactor_debug ${SOURCES})
target_compile_definitions(reactor_debug PUBLIC LTM_DESC USE_LTM DEBUG)
target_link_libraries(reactor_debug ${REACTOR_LIBS})