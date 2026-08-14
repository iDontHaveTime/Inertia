# This CMakeLists.txt file is responsible for building the inr-randir tool.

set(INERTIA_RANDIR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/IR/IRGen.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Driver/Driver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/inr-randir.cpp" # Entry point file.
)

inr_add_tool(inr-randir ${INERTIA_RANDIR_SOURCES})

inr_map_library(RANDIR_REQUIRED_LIBRARIES InrCore InrIR InrCLI)

target_link_libraries(inr-randir PUBLIC
    ${RANDIR_REQUIRED_LIBRARIES}
)

target_include_directories(inr-randir PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)