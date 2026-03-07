# Set the minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

# ./cmake/*
set(INERTIA_CMAKE_UTILS ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Include the version
include(${INERTIA_CMAKE_UTILS}/InertiaVersion.cmake)

# Set the project
project(Inertia
    VERSION ${INERTIA_VERSION_MAJOR}.${INERTIA_VERSION_MINOR}.${INERTIA_VERSION_PATCH}
    LANGUAGES C CXX ASM)

# The backend
add_subdirectory(inr)