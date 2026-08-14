// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_TIR_TIRT_H
#define INERTIA_TIR_TIRT_H

/// @file TIR/TIRT.h
/// @brief Provides a TIR class that represents a type.

namespace inr {

/// @brief Represents a TIR type.
class TIRT {
public:
    enum Kind : unsigned char {
        /// @brief Represents a certain bitwidth type.
        Bit,
        /// @brief Types such as void, block, etc..
        Other,
    };

private:
    union {
        unsigned width_;
    };
    Kind kind_;

    constexpr TIRT(Kind kind) : kind_(kind) {}
    constexpr TIRT(unsigned width) : width_(width), kind_(Bit) {}

public:
    constexpr static TIRT createBit(unsigned width) {
        return TIRT(width);
    }

    constexpr static TIRT createOther() {
        return TIRT(Other);
    }

    unsigned getWidth() const {
        return width_;
    }

    Kind getKind() const {
        return kind_;
    }

    bool operator==(TIRT other) const {
        if(kind_ != other.kind_) return false;
        switch(kind_) {
            case Bit:
                return width_ == other.width_;
            case Other:
                return true;
        }
    }
};

} // namespace inr

#endif // INERTIA_TIR_TIRT_H
