# The modules of Inertia

set(INR_MODULE_FLAGS -Wall -Wextra)

# Support
add_subdirectory(Support)
# Platform
add_subdirectory(Platform)

add_library(InrCore INTERFACE)

target_link_libraries(InrCore INTERFACE
    InrSupport
    InrPlatform
)