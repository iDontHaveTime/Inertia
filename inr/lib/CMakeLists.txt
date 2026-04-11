
# Core library
file(GLOB SupportSources "Support/*.cpp")

add_inertia_library(Core ${SupportSources})

target_compile_definitions(InrCore PRIVATE
    INERTIA_VERSION_MAJOR=${INERTIA_VERSION_MAJOR}
    INERTIA_VERSION_MINOR=${INERTIA_VERSION_MINOR}
    INERTIA_VERSION_PATCH=${INERTIA_VERSION_PATCH}
)

# IR Library
file(GLOB IRSources "IR/*.cpp")

add_inertia_library(IR ${IRSources})

# Should x86 be included in the build
option(BUILD_X86 "Should it include x86" YES)

# Target Library
file(GLOB_RECURSE TargetSources "Target/*.cpp")

add_inertia_library(Target ${TargetSources})

if(BUILD_X86)
    target_compile_definitions(InrTarget PRIVATE
        INERTIA_INCLUDE_X86
    )
endif()

# Option Library
file(GLOB OptionSources "Option/*.cpp")

add_inertia_library(Option ${OptionSources})

# Gen Library
file(GLOB GenSources "Gen/*.cpp")

add_inertia_library(Gen ${GenSources})

# TIR Library
file(GLOB TIRSources "TIR/*.cpp")

add_inertia_library(TIR ${TIRSources})