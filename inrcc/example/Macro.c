// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#define ZERO 0

// BASIC DEFINE TEST
#ifdef ZERO
int main() {
    return ZERO;
}
#endif

#undef ZERO

// AFTER UNDEF CHECK
#ifdef ZERO
#error ZERO should not be defined here
#endif

#ifndef ZERO
#define NONZERO 1
#endif

// SIMPLE BRANCH TEST
#ifdef NONZERO
int foo() {
    return NONZERO;
}
#else
#error NONZERO should be defined here
#endif

// MULTI-BRANCH EXCLUSIVITY TEST
#ifdef ZERO
int should_not_exist_1() { return 1; }
#elif defined(NONZERO)
int ok_branch() { return NONZERO; }
#else
int should_not_exist_2() { return -1; }
#endif

// REDEFINITION BEHAVIOR TEST
#define NONZERO 2

#ifdef NONZERO
int redefined_check() {
    return NONZERO; // should now be 2
}
#endif

// NESTED CONDITIONAL TEST
#ifdef NONZERO
    #ifdef ZERO
        int nested_fail() { return -1; }
    #else
        int nested_ok() { return NONZERO + 10; }
    #endif
#endif

// FINAL FALLBACK BRANCH TEST
#ifndef ZERO
int baz() {
    return 0;
}
#endif

// COMPLEX EXPRESSION TEST
#if defined(NONZERO) && (NONZERO > 0)
int expr_test() {
    return NONZERO * 2;
}
#endif

#ifdef __inrcc__

int using_inrcc() { return 1; }

#endif

#define stdout stdout

#ifdef stdout
void macro_itself() {
    int stdout = 0;
}
#endif

#define ONE_ARG(x) x
#define TWO_ARG(x, y) x + y

#define TOKEN_STACK_TEST() int token_stack_test_var = 5

int one_arg() {
    TOKEN_STACK_TEST();
    return ONE_ARG(5);
}

int one_arg_expr() {
    return ONE_ARG(1 + 2);
}

int add(int x, int y) {
    return TWO_ARG(x, y);
}

#define VARARG_MACRO(...) __VA_ARGS__
#define VARARG_MACRO_WITH_ONE(x, ...) x, __VA_ARGS__

int vararg_test() {
    return VARARG_MACRO(1, 2, 3);
}

int vararg_test_one() {
    return VARARG_MACRO_WITH_ONE(1, 2, 3, 4);
}

#define VARARG_MACRO_NAME(args...) args

int vararg_test_name() {
    return VARARG_MACRO_NAME(1, 2, 3, 4);
}

#define max_macro(a, b) ((a) > (b) ? (a) : (b))

int max(int a, int b) {
    return max_macro(a, b);
}

#define emptyvar(...)
#define emptyvarx(x, ...)

emptyvar()
emptyvarx(1,)

#define str_macro_no_space(x) #x
#define str_macro_yes_space(x) #       x

const char* string_test = "Hello, World!\n";

const char* strm() {
    if(1 > 2) {
        return str_macro_no_space(abcdefg);
    }
    else {
        const char* str = str_macro_no_space("Double string?");
        return str_macro_yes_space(hello world!);
    }
}

#define glue(x, y) x ## y
#define obj_glue a ## b

int gluem() {
    return glue(1, 2);
}

int obj_glue() {
    return -1;
}

#ifdef NOT_DEFINED
#ifdef NOT_DEFINED2
#ifdef NOT_DEFINED3
SHOULDNT EXIST
#endif
#else
SHOULDNT EXIST
#endif
#endif

#if 1 << 3

#endif

#define INNER 42
#define STRINGIFY(x) #x
#define INDIRECT_STRINGIFY(x) STRINGIFY(x)

const char* trap1 = STRINGIFY(INNER);
const char* trap2 = INDIRECT_STRINGIFY(INNER);

#define BACKSLASH_SPLICER(x) #x
const char* spliced = BACKSLASH_SPLICER(line \
split);

#define VARARG_STR(...) #__VA_ARGS__

const char* vararg_str = VARARG_STR(  42,    
     comma,    something, space?   );

#define INRCC_VER(x, y, z) {x, y, z}

struct inrcc_ver {
    int maj, min, pat;
};

#ifndef __inrcc__
#error Not using inrcc
#endif

struct inrcc_ver using_ver() {
    return (struct inrcc_ver)INRCC_VER(__inrcc_major__, __inrcc_minor__, __inrcc_patchlevel__);
}

__SIZE_TYPE__ this_is_the_size_type() {
    return __SHRT_WIDTH__;
}
