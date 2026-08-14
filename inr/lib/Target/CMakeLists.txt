# == InrTarget library ==

inr_add_library(InrTarget
    "${CMAKE_CURRENT_SOURCE_DIR}/Target.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/TargetDesc.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Host.cpp"
)

inr_map_library(TARGET_LIBRARY_NAME InrTarget)

# == InrX86 library ==
# This requires both TIR and Target libraries.
if(INERTIA_TARGET_X86)
    target_compile_definitions(${TARGET_LIBRARY_NAME} PRIVATE "INERTIA_BUILD_X86")
    inr_add_library(InrX86
        "${CMAKE_CURRENT_SOURCE_DIR}/x86/x86Registers.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/x86/x86SystemV.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/x86/x86Info.cpp"
    )
endif()