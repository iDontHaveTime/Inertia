// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_DIAGNOSTICS_DIAGNOSTICS_H
#define INRCC_DIAGNOSTICS_DIAGNOSTICS_H

/// @file Diagnostics/Diagnostics.h
/// @brief Provides a class to handle diagnostics.

#include <inr/ADT/IVector.h>
#include <inr/Support/Stream.h>
#include <inrcc/Driver/DriverFileManager.h>

#include <cstdint>

namespace inrcc {

class Diagnostics {
public:
    enum class Severeness : uint8_t {
        Nothing = 0b00,
        Warning = 0b01,
        Error = 0b10,
        Fatal = 0b11
    };

    enum class Diag : uint16_t {
#define NEW_DIAGNOSTIC(ID, ...) ID,
#include <inrcc/Diagnostics/DiagnosticsList.inc>
#undef NEW_DIAGNOSTIC
        DIAG_LAST
    };

    struct DiagInfo {
        DriverFMan::File where;
        const char* at;
        uint32_t len;
        Diag type;
    };

private:
    inr::ivec<DiagInfo, 8> diagList_;

    Severeness diag_[(unsigned)Diag::DIAG_LAST]{
#define NEW_DIAGNOSTIC(ID, STRID, DEFAULT, ...) DEFAULT,
#include <inrcc/Diagnostics/DiagnosticsList.inc>
#undef NEW_DIAGNOSTIC
    };

public:
    void setSevereness(Diag at, Severeness s) noexcept {
        diag_[(unsigned)at] = s;
    }

    Severeness getSevereness(Diag at) const noexcept {
        return diag_[(unsigned)at];
    }

    void newDiag(DriverFMan::File where, const char* at, uint32_t len,
                 Diag type) noexcept {
        diagList_.emplace_back(where, at, len, type);
    }

    bool hadErrors() const noexcept {
        for(const DiagInfo& info : diagList_)
            if(uint8_t(getSevereness(info.type)) & 0b10) return true;
        return false;
    }

    bool hadFatalErrors() const noexcept {
        for(const DiagInfo& info : diagList_)
            if(getSevereness(info.type) == Diagnostics::Severeness::Fatal)
                return true;
        return false;
    }

    void printall(inr::raw_stream&);
};

} // namespace inrcc

#endif // INRCC_DIAGNOSTICS_DIAGNOSTICS_H
