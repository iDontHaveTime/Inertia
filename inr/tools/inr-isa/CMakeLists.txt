# This CMakeLists.txt file is responsible for building the inr-isa tool.

set(INERTIA_ISA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Lexer/Lexer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Parser/Parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Driver/Diag.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Driver/Emitter.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/Driver/Driver.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/inr-isa.cpp"
)

inr_add_tool(inr-isa ${INERTIA_ISA_SOURCES})

inr_map_library(ISA_REQUIRED_LIBRARIES InrCore InrCLI InrTIR)

target_link_libraries(inr-isa PUBLIC
    ${ISA_REQUIRED_LIBRARIES}
)

target_include_directories(inr-isa PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)