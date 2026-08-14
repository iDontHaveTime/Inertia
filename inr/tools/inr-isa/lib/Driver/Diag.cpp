// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <isa/Driver/Diag.h>

namespace isa {

static inline unsigned getIndent(unsigned long line) {
    if(line < 10UL) return 1;
    if(line < 100UL) return 2;
    if(line < 1000UL) return 3;
    if(line < 10000UL) return 4;
    if(line < 100000UL) return 5;
    if(line < 1000000UL) return 6;
    if(line < 10000000UL) return 7;
    if(line < 100000000UL) return 8;
    if(line < 1000000000UL) return 9;
    if constexpr(sizeof(long) > 4) {
        if(line < 10000000000UL) return 10;
        if(line < 100000000000UL) return 11;
        if(line < 1000000000000UL) return 12;
        if(line < 10000000000000UL) return 13;
        if(line < 100000000000000UL) return 14;
        if(line < 1000000000000000UL) return 15;
        if(line < 10000000000000000UL) return 16;
        if(line < 100000000000000000UL) return 17;
        if(line < 1000000000000000000UL) return 18;
        if(line < 10000000000000000000UL) return 19;
        return 20;
    }
    else return 10;
}

void Diag::printall(inr::stream& os) const {
    unsigned warnings = 0, errors = 0;
    for(const Diagnostic& d : diags_) {
        os.setBold();
        os << d.fileName << ':' << d.line << ':' << d.column << ':';

        switch(d.severity) {
            case Warn:
                warnings++;
                os.changeColor(inr::col::MAGENTA) << " warning: ";
            case Error:
            case Fatal:
                errors++;
                os.changeColor(inr::col::RED) << " error: ";
                break;
        }
        os.removeFColor() << d.message << '\n';

        os.resetColor();

        unsigned indent = getIndent(d.line);

        os.indent(2) << d.line << " | ";
        os << d.span << '\n';
        os.indent(2 + indent) << " | ";
        os.changeColor(inr::col::GREEN, true) << '^';

        for(unsigned i = 1; i < d.span.size(); i++) {
            os << '~';
        }

        os.resetColor() << '\n';
    }

    if(warnings || errors) {
        if(warnings) {
            os << warnings << (((warnings > 1)) ? " warnings" : " warning");
        }
        if(errors) {
            if(warnings) os << " and ";
            os << errors << ((errors > 1) ? " errors" : " error");
        }
        os << " generated.\n";
    }
}

} // namespace isa
