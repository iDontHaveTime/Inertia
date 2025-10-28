# Core cpp files
file(GLOB CORE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(InrCore ${CORE_FILES})

target_compile_options(InrCore PRIVATE
    ${INR_MODULE_FLAGS}
)

target_link_libraries(InrCore PUBLIC InrSupport InrMC)

if(INERTIA_SANITIZE)
    target_compile_options(InrCore PRIVATE
        -fsanitize=address
    )
    target_link_options(InrCore PRIVATE
        -fsanitize=address
    )
endif()