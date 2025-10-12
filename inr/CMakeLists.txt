project(inrlib)

# Default all libraries to shared
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build all libraries as shared by default")

# Make the include path accessible so anyone can include it
set(INERTIA_BACKEND_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
# Make everything under include the include folder
include_directories(${INERTIA_BACKEND_INCLUDE})

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Should tests be included?
option(INERTIA_TEST_EXECUTABLES "Should test executables be included when building" ON)

# Should ASan be used?
option(INERTIA_SANITIZE "Should ASan be used when compiling/linking" ON)

# The core of inertia. modules are subdirectories in lib/
add_subdirectory(lib)
# The tests for the library
add_subdirectory(test)