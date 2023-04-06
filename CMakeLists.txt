cmake_minimum_required(VERSION 3.6)

project(Reactor
        VERSION 1.0.0.0
        DESCRIPTION "A minecraft server in C"
        LANGUAGES C)

add_subdirectory(src)
add_subdirectory(src/net)
add_subdirectory(src/util)
add_subdirectory(src/world)