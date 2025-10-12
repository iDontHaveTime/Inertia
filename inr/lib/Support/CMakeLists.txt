# Get the files
file(GLOB SUPPORT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(InrSupport ${SUPPORT_FILES})

target_compile_options(InrSupport PRIVATE
    ${INR_MODULE_FLAGS}
)

if(INERTIA_SANITIZE)
    target_compile_options(InrSupport PRIVATE
        -fsanitize=address
    )
    target_link_options(InrSupport PRIVATE
        -fsanitize=address
    )
endif()