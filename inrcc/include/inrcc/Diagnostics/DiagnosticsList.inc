
#ifndef NEW_DIAGNOSTIC
#define NEW_DIAGNOSTIC(...)
#define UNDEF_NEW_DIAGNOSTIC
#endif

#define DefNothing Diagnostics::Severeness::Nothing
#define DefWarning Diagnostics::Severeness::Warning
#define DefError Diagnostics::Severeness::Error
#define DefFatal Diagnostics::Severeness::Fatal
#define NoStrID ""

// NEW_DIAGNOSTIC(ID, STRID, DEFAULT, MESSAGE)

/// -Wundef
/// Warns if an undefined macro was found in #if directive.
NEW_DIAGNOSTIC(undef, "undef", DefNothing, "macro is undefined, evaluates to 0")

/// -Wbuiltin-macro-redefined
/// Warns if a builtin macro was redefined
NEW_DIAGNOSTIC(builtin_macro_redefined, "builtin-macro-redefined", DefWarning, "redefining a builtin macro")
/// -Wbuiltin-macro-undef
/// Same as the above just about #undef rather than #define
NEW_DIAGNOSTIC(builtin_macro_undef, "builtin-macro-undefined", DefWarning, "undefining a builtin macro")

/// When include doesn't have <...> or "..."
NEW_DIAGNOSTIC(expected_filename, NoStrID, DefFatal, "expected \"FILE\" or <FILE>")

/// When include doesn't find a file
NEW_DIAGNOSTIC(file_not_found, NoStrID, DefFatal, "file not found")

/// When there is no identifier after a preprocessor directive that requires it
NEW_DIAGNOSTIC(macro_expected_identifier, NoStrID, DefError, "expected an identifier")

/// When there is a new line in a preprocessing directive
NEW_DIAGNOSTIC(sudden_new_line, NoStrID, DefError, "new line encountered")

/// When there is no '>' to close '<'
NEW_DIAGNOSTIC(expected_right_arrow, NoStrID, DefFatal, "expected '>' to close '<'")

/// When there are multiple '...' in macro args.
NEW_DIAGNOSTIC(multiple_ellipsis, NoStrID, DefFatal, "multiple '...' in one definition")

/// For example:
/// #define MACRO(IDENT1, IDENT2 IDENT3)
NEW_DIAGNOSTIC(expected_comma_before_identifier, NoStrID, DefFatal, "expected ',' before identifier")

/// Expected ')' to close macro
NEW_DIAGNOSTIC(macro_expected_rparen, NoStrID, DefFatal, "expected ')' to close macro")

/// For example:
/// #define MACRO(,) or MACRO(IDENT1,,)
NEW_DIAGNOSTIC(comma_after_comma, NoStrID, DefFatal, "multiple ',' found")

/// For example:
/// #foo
NEW_DIAGNOSTIC(unknown_directive, NoStrID, DefError, "unknown directive")

/// For example:
/// #error Some error message
NEW_DIAGNOSTIC(cpp_error, NoStrID, DefFatal, "error directive")

/// For example:
/// #warning Some warning message
NEW_DIAGNOSTIC(cpp_warning, "cpp", DefWarning, "warning directive")

/// Happens when the preprocess had an internal error
NEW_DIAGNOSTIC(if_expr_undone, NoStrID, DefFatal, "expression failed")

/// Happens when you divide by zero
NEW_DIAGNOSTIC(if_expr_div_zero, NoStrID, DefFatal, "division by 0")
/// Same thing but with mod
NEW_DIAGNOSTIC(if_expr_mod_zero, NoStrID, DefFatal, "modulo 0")

/// For example:
/// 1234ABC
NEW_DIAGNOSTIC(invalid_suffix, NoStrID, DefError, "invalid suffix")

/// Expected ':' for ternary expressions in #if
NEW_DIAGNOSTIC(macro_expected_colon, NoStrID, DefFatal, "expected ':' for '?'")

NEW_DIAGNOSTIC(macro_expected_lparen, NoStrID, DefFatal, "expected '(' for expression")

/// Known prefixes: L, U, u, u8
NEW_DIAGNOSTIC(unknown_string_prefix, NoStrID, DefFatal, "unknown string prefix")
/// Same thing as above but for char literals
NEW_DIAGNOSTIC(unknown_charl_prefix, NoStrID, DefFatal, "unknown char literal prefix")

NEW_DIAGNOSTIC(string_not_closed, NoStrID, DefFatal, "string wasn't closed properly")
NEW_DIAGNOSTIC(char_not_closed, NoStrID, DefFatal, "char literal wasn't closed properly")

#undef DefNothing
#undef DefWarning
#undef DefError
#undef DefFatal
#undef NoStrID

#ifdef UNDEF_NEW_DIAGNOSTIC
#undef UNDEF_NEW_DIAGNOSTIC
#undef NEW_DIAGNOSTIC
#endif