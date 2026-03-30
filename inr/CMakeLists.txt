# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Default to shared libraries
option(BUILD_SHARED_LIBS "Build as shared libraries" YES)

# Compile with warning flags
option(BUILD_WARNINGS "Build with warning flags" YES)

# ./inr/include
set(INERTIA_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(map_name_to_library var_to_set)
    set(temp_list "")
    foreach(lib_name ${ARGN})
        list(APPEND temp_list "Inr${lib_name}")
    endforeach()

    set(${var_to_set} ${temp_list} PARENT_SCOPE)
endfunction()

# This will hold all libraries created with add_inertia_library
map_name_to_library(INERTIA_LIBRARIES Core IR Target Option Gen)

# add_inertia_library(NAME files...)
function(add_inertia_library lib_name)
    if(NOT ARGN)
        set(lib_srcs "")
    else()
        set(lib_srcs ${ARGN})
    endif()

    add_library("Inr${lib_name}" ${lib_srcs})

    target_include_directories("Inr${lib_name}" PUBLIC ${INERTIA_INCLUDE_DIRS})

    if(BUILD_WARNINGS)
        target_compile_options("Inr${lib_name}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(tools)