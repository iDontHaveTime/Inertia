
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

# Target Library
file(GLOB_RECURSE TargetSources "Target/*.cpp")

add_inertia_library(Target ${TargetSources})

# Option Library
file(GLOB OptionSources "Option/*.cpp")

add_inertia_library(Option ${OptionSources})

# Gen Library
file(GLOB GenSources "Gen/*.cpp")

add_inertia_library(Gen ${GenSources})