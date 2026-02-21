# Get the files
file(GLOB PLATFORM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(InrPlatform ${PLATFORM_FILES})

target_compile_options(InrPlatform PRIVATE
    ${INR_MODULE_FLAGS}
)

if(INERTIA_SANITIZE)
    target_compile_options(InrPlatform PRIVATE
        -fsanitize=address
    )
    target_link_options(InrPlatform PRIVATE
        -fsanitize=address
    )
endif()