cmake_minimum_required(VERSION 2.6)

project(LIBIBUDDY)
add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(tools)

set(CMAKE_CXX_FLAGS "-g -Wall")

