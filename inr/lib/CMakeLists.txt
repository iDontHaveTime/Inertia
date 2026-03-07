
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
file(GLOB ISelSources "ISel/*.cpp")

add_inertia_library(ISel ${ISelSources})