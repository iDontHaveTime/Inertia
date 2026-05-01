// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_OPTIONS_DATA_H
#define INRCC_OPTIONS_DATA_H

/// @file Options/Data.h
/// @brief Represents some abstraction over data.

#include <inr/Target/Triple.h>

namespace inrcc {

#ifndef INRCC_INACCURATE
#define INRCC_BITDIV(OF, BY) OF / BY
#else
#define INRCC_BITDIV(OF, BY) OF >> 3
#endif

class CData {
    unsigned charW_;
    unsigned shortW_;
    unsigned intW_;
    unsigned longW_;
    unsigned llW_;
    unsigned ptrW_;
    unsigned sizeW_;

public:
    CData(inr::Triple);

    unsigned getCharWidth() const noexcept {
        return charW_;
    }

    unsigned getShortWidth() const noexcept {
        return shortW_;
    }

    unsigned getIntWidth() const noexcept {
        return intW_;
    }

    unsigned getLongWidth() const noexcept {
        return longW_;
    }

    unsigned getLongLongWidth() const noexcept {
        return llW_;
    }

    unsigned getPtrWidth() const noexcept {
        return ptrW_;
    }

    unsigned getSizeWidth() const noexcept {
        return sizeW_;
    }

    unsigned getCharSize() const noexcept {
        return 1;
    }

    unsigned getShortSize() const noexcept {
        return INRCC_BITDIV(shortW_, charW_);
    }

    unsigned getIntSize() const noexcept {
        return INRCC_BITDIV(intW_, charW_);
    }

    unsigned getLongSize() const noexcept {
        return INRCC_BITDIV(longW_, charW_);
    }

    unsigned getLongLongSize() const noexcept {
        return INRCC_BITDIV(llW_, charW_);
    }

    unsigned getPtrSize() const noexcept {
        return INRCC_BITDIV(ptrW_, charW_);
    }

    unsigned getSizeSize() const noexcept {
        return INRCC_BITDIV(sizeW_, charW_);
    }
};

#undef INRCC_BITDIV

} // namespace inrcc

#endif // INRCC_OPTIONS_DATA_H
