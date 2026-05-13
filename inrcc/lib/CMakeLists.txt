
# Options library
file(GLOB OptionsSources "Options/*.cpp")

add_inrcc_library(Options ${OptionsSources})

# Lexer library
file(GLOB LexerSources "Lexer/*.cpp")

add_inrcc_library(Lexer ${LexerSources})

target_compile_definitions(InrccLexer PRIVATE
    INRCC_VERSION_MAJOR=${INERTIA_VERSION_MAJOR}
    INRCC_VERSION_MINOR=${INERTIA_VERSION_MINOR}
    INRCC_VERSION_PATCH=${INERTIA_VERSION_PATCH}
)

# Driver library
file(GLOB DriverSources "Driver/*.cpp")

add_inrcc_library(Driver ${DriverSources})

# Diagnostics library
file(GLOB DiagSources "Diagnostics/*.cpp")

add_inrcc_library(Diag ${DiagSources})

# Parser library
file(GLOB ParserSources "Parser/*.cpp")

add_inrcc_library(Parser ${ParserSources})

# Sema library
file(GLOB SemaSources "Sema/*.cpp")

add_inrcc_library(Sema ${SemaSources})
