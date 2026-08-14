# == InrIR library ==

inr_add_library(InrIR
    "${CMAKE_CURRENT_SOURCE_DIR}/TUnit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FuncDef.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InstDef.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TypeMap.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Printer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Verifier.cpp"
)