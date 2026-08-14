# This CMakeLists.txt file is responsible for building the test executables.

# Match the library's standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(TESTING_LIBRARIES_EXTRA)

if(INERTIA_TARGET_X86)
    set(TESTING_LIBRARIES_EXTRA
        ${TESTING_LIBRARIES_EXTRA} InrX86
    )
endif()

inr_map_library(TESTING_LIBRARIES_LINK InrCore InrIR InrTarget InrTIR ${TESTING_LIBRARIES_EXTRA})

function(inr_make_test TestSource)
    set(options "")
    set(oneValueArgs EXPECT_OUTPUT)
    set(multiValueArgs "")

    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    get_filename_component(exe_name ${TestSource} NAME_WE)

    add_executable(${exe_name} ${TestSource})
    target_link_libraries(${exe_name} PRIVATE ${TESTING_LIBRARIES_LINK})
    target_include_directories(${exe_name} PRIVATE ${INERTIA_INCLUDE_DIRS})
    
    add_test(NAME ${exe_name} COMMAND ${exe_name})

    if(TEST_EXPECT_OUTPUT)
        set_tests_properties(${exe_name} PROPERTIES 
            PASS_REGULAR_EXPRESSION "${TEST_EXPECT_OUTPUT}"
        )
    endif()
endfunction()

# Test the assertion header.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/AssertTest.cpp")

# Test the stdout stream.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/StdoutTest.cpp" EXPECT_OUTPUT "Hello, World!")

# Testing the stream's flushing.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/StreamFlush.cpp" EXPECT_OUTPUT "Some example")

# Testing other streams besides the main ones.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/StreamVariation.cpp")

# Testing path utils.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/PathTest.cpp")

# Vfs library test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/FSTest.cpp")

# Inline vector testing.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/IVecTest.cpp")

# Arbitrary precision integer testing.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/BigIntTest.cpp")

# IR's TypeMap class test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/TypeMapTest.cpp")

# Hash map test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/HMapTest.cpp")

# Hash set test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/HSetTest.cpp")

# Intrusive linked list test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/IListTest.cpp")

# Printing the IR test.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/IRPrintTest.cpp")

# Testing whether the host detection works.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/TargetTest.cpp")

# Testing IR to TIR translation.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/TIRTest.cpp")

# Testing integer arithmetic lowering.
inr_make_test("${CMAKE_CURRENT_SOURCE_DIR}/ArithmeticTest.cpp")