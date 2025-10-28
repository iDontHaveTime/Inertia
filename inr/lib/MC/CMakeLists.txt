# MC cpp files
file(GLOB CORE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(InrMC ${CORE_FILES})

target_compile_options(InrMC PRIVATE
    ${INR_MODULE_FLAGS}
)

target_link_libraries(InrMC PUBLIC InrSupport)

if(INERTIA_SANITIZE)
    target_compile_options(InrMC PRIVATE
        -fsanitize=address
    )
    target_link_options(InrMC PRIVATE
        -fsanitize=address
    )
endif()