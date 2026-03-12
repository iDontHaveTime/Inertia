
# Core library
file(GLOB SupportSources "Support/*.cpp")

add_inertia_library(Core ${SupportSources})

# IR Library
file(GLOB IRSources "IR/*.cpp")

add_inertia_library(IR ${IRSources})

# Target Library
file(GLOB TargetSources "Target/*.cpp")

add_inertia_library(Target ${TargetSources})

# ISel Library
file(GLOB_RECURSE ISelSources "ISel/*.cpp")

add_inertia_library(ISel ${ISelSources})

# Option Library
file(GLOB OptionSources "Option/*.cpp")

add_inertia_library(Option ${OptionSources})

# Gen Library
file(GLOB GenSources "Gen/*.cpp")

add_inertia_library(Gen ${GenSources})