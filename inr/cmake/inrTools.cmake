# Which tools should be included in the build.

# Random IR generator tool.
# This tool generates a random IR sample, mainly used for testing.
option(INERTIA_TOOL_RANDIR "Should inr-randir be included in the build?" ON)

# inr-isa tool.
# Generates C++ files from .isa files.
option(INERTIA_TOOL_ISA "Should inr-isa be included in the build?" ON)
