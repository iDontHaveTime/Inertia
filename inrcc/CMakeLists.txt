cmake_minimum_required(VERSION 3.20.0)

# inrcc/cmake/*
set(INRCC_CMAKE_UTILS "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("${INRCC_CMAKE_UTILS}/inrccVersion.cmake")

project(inrcc
    VERSION ${INRCC_VERSION_MAJOR}.${INRCC_VERSION_MINOR}.${INRCC_VERSION_PATCH}
    LANGUAGES CXX)
