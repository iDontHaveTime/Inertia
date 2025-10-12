
if(INERTIA_TEST_EXECUTABLES)
    # test cpp files
    file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

    foreach(test_file IN LISTS TEST_SOURCES)
        get_filename_component(test_name ${test_file} NAME_WE)
        add_executable(${test_name} ${test_file})

        target_link_libraries(${test_name} PRIVATE InrSupport)

        if(INERTIA_SANITIZE)
            target_compile_options(${test_name} PRIVATE
                -fsanitize=address
            )
            target_link_options(${test_name} PRIVATE
                -fsanitize=address
            )
        endif()

    endforeach()

endif()