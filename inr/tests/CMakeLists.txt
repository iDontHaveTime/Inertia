# This CMakeLists.txt file is responsible for building the test executables.

# Match the library's standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

inr_map_library(TESTING_LIBRARIES_LINK InrCore InrIR InrTarget)

function(inr_make_test TestSource)
    get_filename_component(exe_name ${TestSource} NAME_WE)

    add_executable(${exe_name} ${TestSource})
    target_link_libraries(${exe_name} PRIVATE ${TESTING_LIBRARIES_LINK})
    target_include_directories(${exe_name} PRIVATE ${INERTIA_INCLUDE_DIRS})

    add_test(NAME ${exe_name} COMMAND ${exe_name})
endfunction()

file(GLOB_RECURSE CPP_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/cpp/*.cpp")

foreach(CPP_TEST_FILE IN LISTS CPP_TEST_FILES)
    inr_make_test(${CPP_TEST_FILE})
endforeach()