
# add_test_executable(X.cpp)
function(add_test_executable TEST_SOURCE)
    get_filename_component(FILE_NO_EXT ${TEST_SOURCE} NAME_WE)

    add_executable(${FILE_NO_EXT} ${TEST_SOURCE})

    target_include_directories(${FILE_NO_EXT} PRIVATE ${INERTIA_INCLUDE_DIRS})
    
    target_link_libraries(${FILE_NO_EXT} PRIVATE ${INERTIA_LIBRARIES})
endfunction()

add_test_executable(StreamTest.cpp)
add_test_executable(LoggerTest.cpp)
add_test_executable(ModuleTest.cpp)
add_test_executable(TargetTreeTest.cpp)