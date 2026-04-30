
function(add_inrcc_exec SOURCE_FILE)
    get_filename_component(FILE_NO_EXT ${SOURCE_FILE} NAME_WE)

    add_executable(${FILE_NO_EXT} ${SOURCE_FILE})

    target_include_directories(${FILE_NO_EXT} PRIVATE ${INRCC_INCLUDE_DIRS})

    target_link_libraries(${FILE_NO_EXT} PRIVATE ${INRCC_LIBRARIES})
endfunction()

add_inrcc_exec(inrcc.cpp)