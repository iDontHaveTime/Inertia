# Main CMakeLists.txt file for Inertia tools directory.

# Tools should match Inertia's required C++ standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(inr_add_tool TOOL_NAME)
    add_executable(${TOOL_NAME} ${ARGN})

    target_include_directories(${TOOL_NAME} PUBLIC ${INERTIA_INCLUDE_DIRS})

    if(MSVC)
        target_compile_options(${TOOL_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TOOL_NAME} PRIVATE -Wall -Wextra)
    endif()

endfunction()

# Random IR generation tool.
if(INERTIA_TOOL_RANDIR)
    add_subdirectory(inr-randir)
endif()

# ISA lang.
if(INERTIA_TOOL_ISA)
    add_subdirectory(inr-isa)
endif()