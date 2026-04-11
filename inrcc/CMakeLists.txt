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

