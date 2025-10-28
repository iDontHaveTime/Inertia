#ifndef INERTIA_ATTRIBUTE_HPP
#define INERTIA_ATTRIBUTE_HPP

/**
 * @file inr/Defines/Attribute.hpp
 * @brief Inertia's attribute macros.
 *
 * This header contains macros to make the code more understandable to the compiler.
 *
 **/

#if defined(_MSC_VER)
#define INR_MSVC
#endif // msvc

#if defined(__GNUC__) || defined(__clang__)
#define INR_GNU
#endif // gnu c

/* Check builtin. */
#if defined(__has_builtin)
#define _inr_check_builtin_(x) __has_builtin(x)
#else
#define _inr_check_builtin_(x) 0
#endif // check builtin

/* Check attribute. */
#if defined(__has_attribute)
#define _inr_check_attribute_(x) __has_attribute(x)
#else
#define _inr_check_attribute_(x) 0
#endif

#if _inr_check_attribute_(packed)
#define _inr_packed_ __attribute__((packed))
#else
#define _inr_packed_
#endif

/* Always inline. */
#if __has_cpp_attribute(__gnu__::__always_inline__)
#define _inr_always_inline_ [[__gnu__::__always_inline__]]
#else
#define _inr_always_inline_
#endif // always inline

#ifdef INR_MSVC
#undef INR_MSVC
#endif // INR_MSVC

/* Restrict. */
#ifdef INR_MSVC // msvc
#define _inr_restrict_ __restrict
#elif defined(INR_GNU) // gnu
#define _inr_restrict_ __restrict__
#else // no gnu or msvc
#define _inr_restrict_
#endif // restrict

#ifdef INR_GNU
#undef INR_GNU
#endif // INR_GNU

#endif // INERTIA_ATTRIBUTE_HPP
