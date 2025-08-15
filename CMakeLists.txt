cmake_minimum_required(VERSION 3.10)
project(Inertia LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()

add_subdirectory(Inertia)
add_subdirectory(inrcc)