cmake_minimum_required(VERSION 3.10.0)

project(Inertia VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB_RECURSE LIB_SOURCES "lib/Inertia/*.cpp")
file(GLOB_RECURSE FRONTEND_SOURCES "src/main.cpp")

set(SANITIZE_LIBRARY ON)
set(SANITIZE_EXECUTABLE ON)

add_library(InertiaLib SHARED ${LIB_SOURCES})

target_compile_options(InertiaLib PUBLIC -Wall -Wextra)
target_include_directories(InertiaLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(InertiaLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

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