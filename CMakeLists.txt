cmake_minimum_required(VERSION 3.10.0)

project(Inertia VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIB_SOURCES "lib/Inertia/*.cpp")
file(GLOB_RECURSE FRONTEND_SOURCES "src/main.cpp")

if(CMAKE_CXX_COMPILER STREQUAL "clang++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=lld")
endif()

set(SANITIZE_LIBRARY ON)
set(SANITIZE_EXECUTABLE ON)
set(LTO_LIBRARY OFF)

add_library(InertiaLib SHARED ${LIB_SOURCES})

target_compile_options(InertiaLib PUBLIC -Wall -Wextra)
target_include_directories(InertiaLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(LTO_LIBRARY)
    set_target_properties(InertiaLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(SANITIZE_LIBRARY)
    target_compile_options(InertiaLib PRIVATE -fsanitize=address)
    target_link_options(InertiaLib PRIVATE -fsanitize=address)
endif()

add_executable(Inertia ${FRONTEND_SOURCES})

target_link_libraries(Inertia PRIVATE InertiaLib)

target_include_directories(Inertia PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(SANITIZE_EXECUTABLE)
    target_compile_options(Inertia PRIVATE -fsanitize=address)
    target_link_options(Inertia PRIVATE -fsanitize=address)
endif()