#ifndef INERTIA_OPTION_ARG_H
#define INERTIA_OPTION_ARG_H

/// @file Option/Arg.h
/// @brief Contains the arg class.

#include <inr/Option/Option.h>

#include <vector>

namespace inr::opt {

/// @brief Abstract argument for a driver.
class Arg {
    /// @brief Options of the arg.
    Option* opt_;

    /// @brief Where the arg is in the list.
    unsigned index_;

    /// @brief Whether the flag was used or not.
    bool claim_;

    /// @brief The pointer to the original string of the arg.
    sview argStr_;

    std::vector<const char*> values_;

    Arg(sview argStr, Option* opt, unsigned index,
        std::vector<const char*> values) noexcept :
        opt_(opt), index_(index), argStr_(argStr), values_(std::move(values)) {}

public:
    const Option& getOption() const noexcept {
        return *opt_;
    }

    sview getArgStr() const noexcept {
        return argStr_;
    }

    unsigned getIndex() const noexcept {
        return index_;
    }

    void claim() noexcept {
        claim_ = true;
    }

    bool isClaimed() const noexcept {
        return claim_;
    }

    Option::OptionID getID() const noexcept {
        return opt_->getID();
    }

    const char* getValue(unsigned n = 0) const noexcept {
        return values_[n];
    }

    const std::vector<const char*>& getValues() const noexcept {
        return values_;
    }

    friend class ArgList;
};

} // namespace inr::opt

#endif // INERTIA_OPTION_ARG_H
