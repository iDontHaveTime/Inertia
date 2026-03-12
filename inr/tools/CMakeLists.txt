
function(add_inertia_cli_tool TOOL_NAME)
    get_filename_component(FILE_NO_EXT ${TOOL_NAME} NAME_WE)

    add_executable(${FILE_NO_EXT} ${TOOL_NAME})

    target_include_directories(${FILE_NO_EXT} PRIVATE ${INERTIA_INCLUDE_DIRS})

    target_link_libraries(${FILE_NO_EXT} PRIVATE ${INERTIA_LIBRARIES})
endfunction()

add_inertia_cli_tool(inr-gen.cpp)