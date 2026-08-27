# This CMakeLists.txt file is responsible for building the Inertia libraries.

# C++20 without GNU extensions.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(INERTIA_MONOLITHIC)
    if(INERTIA_SHARED)
        add_library(Inr SHARED)
    else()
        add_library(Inr)
    endif()

    target_include_directories(Inr PUBLIC ${INERTIA_INCLUDE_DIRS})

    if(MSVC)
        target_compile_options(Inr PRIVATE /W4)
    else()
        target_compile_options(Inr PRIVATE -Wall -Wextra)
    endif()

    macro(inr_add_library TARGET_NAME)
        target_sources(Inr PRIVATE "${ARGN}")
    endmacro()
else()
    # A function that creates a new libraries and includes directories, 
    # sets the standard, and also handles shared vs static libs.
    function(inr_add_library TARGET_NAME)

        if(INERTIA_SHARED)
            add_library(${TARGET_NAME} SHARED ${ARGN})
        else()
            add_library(${TARGET_NAME} ${ARGN})
        endif()

        target_include_directories(${TARGET_NAME} PUBLIC ${INERTIA_INCLUDE_DIRS})

        if(MSVC)
            target_compile_options(${TARGET_NAME} PRIVATE /W4)
        else()
            target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra)
        endif()

    endfunction()
endif()

set(INERTIA_LIB_FILES ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Support)
add_subdirectory(CLI)
add_subdirectory(IR)
add_subdirectory(Target)
