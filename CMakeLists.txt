cmake_minimum_required(VERSION 3.5)

project(uasat VERSION 0.1
    DESCRIPTION "SAT based universal algebra calculator in C++"
    LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_subdirectory(src)
add_subdirectory(apps)
