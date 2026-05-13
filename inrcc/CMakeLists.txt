# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Would be better to use as NO on final builds
option(BUILD_SHARED_LIBS "Build as shared libraries" YES)

# Same as inr for now
option(BUILD_WARNINGS "Build with warning flags" YES)

# ./inrcc/include
set(INRCC_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# This will hold all libraries created with add_inrcc_library
inrcc_map_name_to_library(INRCC_LIBRARIES Options Lexer Driver Diag Parser Sema)

# These are the Inertia libraries inrcc uses
inr_map_name_to_library(INRCC_NEEDED_LIBRARIES Core IR Target Option TIR)

# add_inrcc_library(NAME files...)
function(add_inrcc_library lib_name)
    if(NOT ARGN)
        set(lib_srcs "")
    else()
        set(lib_srcs ${ARGN})
    endif()

    add_library("Inrcc${lib_name}" ${lib_srcs})

    target_include_directories("Inrcc${lib_name}" PUBLIC ${INRCC_INCLUDE_DIRS})
    target_link_libraries("Inrcc${lib_name}" PUBLIC ${INRCC_NEEDED_LIBRARIES})

    if(BUILD_WARNINGS)
        target_compile_options("Inrcc${lib_name}" PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

add_subdirectory(lib)
add_subdirectory(tools)