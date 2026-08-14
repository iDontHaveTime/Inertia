# == InrCore library ==
# This library should be the Support, Math, and Vfs folders.

configure_file(
    "${INERTIA_LIB_FILES}/Support/Version.cpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp"
    @ONLY
)

inr_add_library(InrCore
    "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp"
    "${INERTIA_LIB_FILES}/Support/Assert.cpp"
    "${INERTIA_LIB_FILES}/Support/Stream.cpp"
    "${INERTIA_LIB_FILES}/Support/Unreachable.cpp"
    "${INERTIA_LIB_FILES}/Math/BigInt.cpp"
    "${INERTIA_LIB_FILES}/Vfs/FStream.cpp"
    "${INERTIA_LIB_FILES}/Vfs/Path.cpp"
    "${INERTIA_LIB_FILES}/Vfs/Vfs.cpp"
)