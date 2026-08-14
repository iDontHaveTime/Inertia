// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DRIVER_DIAG
#define INERTIA_DRIVER_DIAG

/// @file Driver/Diag.h
/// @brief Provides diagnostics for components of `inr-isa`.

#include <inr/Support/Stream.h>

#include <string>
#include <string_view>
#include <vector>

namespace isa {

class Diag {
public:
    enum Severity {
        Warn,  ///< Displays a warning.
        Error, ///< Displays an error.
        Fatal, ///< Displays an error and stops at the stage.
    };
    struct Diagnostic {
        Severity severity;
        std::string_view span;
        std::string message;
        unsigned long line;
        unsigned long column;
        std::string_view fileName;
    };

    struct DiagInfo {
        unsigned warnings = 0;
        unsigned errors = 0;
        unsigned fatals = 0;
    };

private:
    std::vector<Diagnostic> diags_;

public:
    void newDiagnostic(Severity sev, std::string_view span, std::string msg,
                       unsigned long line, unsigned long column,
                       std::string_view fileName) {
        diags_.emplace_back(sev, span, std::move(msg), line, column, fileName);
    }

    DiagInfo getInfo() const {
        DiagInfo info;
        for(const Diagnostic& d : diags_) {
            switch(d.severity) {
                case Warn:
                    info.warnings++;
                    break;
                case Error:
                    info.errors++;
                    break;
                case Fatal:
                    info.fatals++;
                    break;
            }
        }
        return info;
    }

    void printall(inr::stream&) const;
};

} // namespace isa

#endif // INERTIA_DRIVER_DIAG
