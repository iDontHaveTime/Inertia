
set(INERTIA_BUILD "MultiShared" CACHE STRING "How should Inertia be built")

set_property(CACHE INERTIA_BUILD PROPERTY STRINGS "MultiShared" "Shared" "Static")

set(INERTIA_BUILD_VALID "MultiShared" "Shared" "Static")
if(NOT INERTIA_BUILD IN_LIST INERTIA_BUILD_VALID)
    message(FATAL_ERROR 
        "Invalid build mode selected, possible build modes are ${INERTIA_BUILD_VALID}"
    )
endif()