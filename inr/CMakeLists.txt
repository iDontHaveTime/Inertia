# The root cmake for the Inertia backend.
cmake_minimum_required(VERSION 3.20.0)

# == Utils ==

# inr/cmake/*
set(INERTIA_CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include the version.
include("${INERTIA_CMAKE_UTILS}/inrVersion.cmake")
# Include the build mode.
include("${INERTIA_CMAKE_UTILS}/inrBuild.cmake")

# Set the variables so that its easier to check the build mode.
if(INERTIA_BUILD STREQUAL "Shared")
    set(INERTIA_SHARED TRUE)
    set(INERTIA_MONOLITHIC TRUE)
elseif(INERTIA_BUILD STREQUAL "Static")
    set(INERTIA_MONOLITHIC TRUE)
elseif(INERTIA_BUILD STREQUAL "MultiShared")
    set(INERTIA_SHARED TRUE)
endif()

# Uses monolithic library name if Inertia was built that way.
macro(inr_map_library VARIABLE_TO_SET)
    if(INERTIA_MONOLITHIC)
        set(${VARIABLE_TO_SET} Inr)
    else()
        set(${VARIABLE_TO_SET} "${ARGN}")
    endif()
endmacro()

# == Libraries ==

# inr/include
set(INERTIA_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include")

project(Inertia
    VERSION ${INERTIA_VERSION_MAJOR}.${INERTIA_VERSION_MINOR}.${INERTIA_VERSION_PATCH}
    LANGUAGES CXX)

# Source of the libraries
add_subdirectory(lib)

# == Tooling ==

include("${INERTIA_CMAKE_UTILS}/inrTools.cmake")

add_subdirectory(tools)

# == Testing ==

option(INERTIA_ENABLE_TESTING "Should testing be enabled?" ON)

if(INERTIA_ENABLE_TESTING)
    enable_testing()
    # Test executables
    add_subdirectory(tests)
endif()