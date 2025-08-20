
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIB_SOURCES "lib/inrcc/*.cpp")

set(SANITIZE_INRCC ON)

add_library(InrccLib SHARED ${LIB_SOURCES} ${TARGET_SRCS})

set(LTO_INRCC OFF)

target_link_libraries(InrccLib PUBLIC InertiaLib)

target_compile_options(InrccLib PRIVATE -Wall -Wextra)
target_include_directories(InrccLib PUBLIC "include")

if(LTO_INRCC)
    set_target_properties(InrccLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(SANITIZE_INRCC)
    target_compile_options(InrccLib PRIVATE -fsanitize=address)
    target_link_options(InrccLib PRIVATE -fsanitize=address)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_link_options(InrccLib PUBLIC -fuse-ld=lld)
endif()

file(GLOB_RECURSE DRIVER_SOURCES "tools/driver/*.cpp")

add_executable(inrccdriver ${DRIVER_SOURCES})
target_link_libraries(inrccdriver PUBLIC InrccLib)
target_compile_options(inrccdriver PRIVATE -Wall -Wextra)

if(SANITIZE_INRCC)
    target_compile_options(inrccdriver PRIVATE -fsanitize=address)
    target_link_options(inrccdriver PRIVATE -fsanitize=address)
endif()
