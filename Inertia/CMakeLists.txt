
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE LIB_SOURCES "lib/Inertia/*.cpp")

set(SANITIZE_LIBRARY ON)

set(ADD_TARGETS OFF)

set(TARGET_SRCS "")
if(ADD_TARGETS)
    file(GLOB_RECURSE TARGET_SRCS "include/Inertia/Target/*.cpp")
endif()

add_library(InertiaLib SHARED ${LIB_SOURCES} ${TARGET_SRCS})

set(LTO_LIBRARY OFF)

target_compile_options(InertiaLib PRIVATE -Wall -Wextra)
target_include_directories(InertiaLib PUBLIC "include")

if(LTO_LIBRARY)
    set_target_properties(InertiaLib PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if(SANITIZE_LIBRARY)
    target_compile_options(InertiaLib PRIVATE -fsanitize=address)
    target_link_options(InertiaLib PRIVATE -fsanitize=address)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(InertiaLib PUBLIC -stdlib=libc++)
    target_link_options(InertiaLib PUBLIC -stdlib=libc++ -fuse-ld=lld)
endif()

set(COMPILE_TEST_EXECUTABLE ON)

if(COMPILE_TEST_EXECUTABLE)
    set(TEST_MAIN "src/main.cpp")
    add_executable(InertiaTest ${TEST_MAIN})
    target_link_libraries(InertiaTest PRIVATE InertiaLib)
    target_include_directories(InertiaTest PRIVATE "include")
    target_compile_options(InertiaTest PRIVATE -Wall -Wextra)
    if(SANITIZE_LIBRARY)
        target_compile_options(InertiaTest PRIVATE -fsanitize=address)
        target_link_options(InertiaTest PRIVATE -fsanitize=address)
    endif()
endif()