#ifndef INERTIA_OPTION_OPTION_H
#define INERTIA_OPTION_OPTION_H

/// @file Option/Option.h
/// @brief Contains the option class.

#include <inr/ADT/StrView.h>

#include <cstdint>

namespace inr::opt {

class Option {
public:
    /// @brief
    enum class OptionID : uint8_t {
        Uknown,          ///< Unknown arg provided.
        Input,           ///< Input file arg (e.g. foo.c, bar.cpp).
        Flag,            ///< Flag arg (e.g. --help, --version).
        Values,          ///< Values arg (e.g. -std=c17, -std=c++20).
        Joined,          ///< Joined arg (e.g. -ofoo.o, -I/usr/include)
        Separate,        ///< Separate arg (e.g. -o file, -L /usr/lib)
        JoinedOrSeparate ///< Either joined or separate arg.
    };

private:
    /// @brief Type of the option.
    OptionID id_;
    /// @brief The prefix of the option (e.g. -, --).
    const char* prefix_;
    /// @brief The name of the option.
    const char* flag_;
    /// @brief The text that displays if printHelp() is used.
    const char* helpText_;

public:
    constexpr Option(OptionID id, const char* prefix, const char* flag,
                     const char* helpText) noexcept :
        id_(id), prefix_(prefix), flag_(flag), helpText_(helpText) {}

    constexpr bool hasPrefix() const noexcept {
        return prefix_ != nullptr;
    }

    constexpr sview getPrefix() const noexcept {
        return sview(prefix_);
    }

    constexpr sview getFlag() const noexcept {
        return sview(flag_);
    }

    constexpr sview getHelp() const noexcept {
        return sview(helpText_);
    }

    constexpr OptionID getID() const noexcept {
        return id_;
    }
};

} // namespace inr::opt

#endif // INERTIA_OPTION_OPTION_H
