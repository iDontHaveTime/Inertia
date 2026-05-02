// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inrcc/Diagnostics/Diagnostics.h>

namespace inrcc {

struct Position {
    unsigned long line = 1;
    unsigned long column = 1;
};

static inline Position calculateNLandColumn(inr::sview from, const char* src) {
    Position pos;

    for(auto it = from.begin(); it != from.end(); ++it) {
        if(it == src) return pos;
        if(*it == '\n') {
            pos.line++;
            pos.column = 1;
        }
        else {
            pos.column++;
        }
    }

    return pos;
}

static inline inr::sview getSpan(inr::sview file, const char* at,
                                 uint32_t len) {
    const char *start = at, *end = at + len;

    while(start != file.data()) {
        if(*(start - 1) == '\n') {
            break;
        }
        start--;
    }

    while(end != file.end()) {
        if(!*end) break;
        if(*end == '\n') break;
        end++;
    }

    return {start, end};
}

unsigned getIndent(unsigned long line) {
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

void Diagnostics::printall(inr::raw_stream& os) {
    unsigned warnings = 0, errors = 0;
    for(const DiagInfo& info : diagList_) {
        Severeness sev = getSevereness(info.type);
        if(sev == Severeness::Nothing) continue;
        os.setBold();

        os << info.where.originalName << ':';
        Position pos =
            calculateNLandColumn(info.where.file->memfile.view(), info.at);
        os << pos.line << ':' << pos.column << ':';

        switch(sev) {
            case Severeness::Warning:
                warnings++;
                os.changeColor(inr::col::YELLOW) << " warning: ";
                break;
            case Severeness::Error:
                [[fallthrough]];
            case Severeness::Fatal:
                errors++;
                os.changeColor(inr::col::RED) << " error: ";
                break;
            case Severeness::Nothing:
                inr_notpossible("This is checked beforehand");
        }
        os.removeFColor();

        switch(info.type) {
#define NEW_DIAGNOSTIC(ID, STRID, DEFAULT, MESSAGE) \
    case Diag::ID:                                  \
        if constexpr(sizeof(STRID) > 1) {           \
            os << MESSAGE " [-W" STRID "]\n";       \
        }                                           \
        else os << MESSAGE "\n";                    \
        break;
#include <inrcc/Diagnostics/DiagnosticsList.inc>
#undef NEW_DIAGNOSTIC
            case Diag::DIAG_LAST:
                os << "unknown_case\n";
        }

        os.resetColor();

        inr::sview msgSpan =
            getSpan(info.where.file->memfile.view(), info.at, info.len);

        unsigned long maxLine = pos.line + msgSpan.count('\n');
        unsigned indent = getIndent(maxLine);

        char buffer[64];
        auto res = std::to_chars(buffer, buffer + sizeof(buffer), pos.line);
        if(res.ec != std::errc()) continue;
        size_t len = res.ptr - buffer;
        os.indent(2) << inr::sview(buffer, len);
        os.indent(indent - len) << " | ";
        for(char c : msgSpan) {
            if(c == '\t') {
                os.indent(4);
            }
            else {
                os << c;
            }
            if(c == '\n') {
                pos.line++;
                res = std::to_chars(buffer, buffer + sizeof(buffer), pos.line);
                if(res.ec != std::errc()) continue;
                len = res.ptr - buffer;
                os.indent(2) << inr::sview(buffer, len);
                os.indent(1 + (indent - len)) << "| ";
            }
        }
        os << '\n';

        os.indent(indent) << "   | ";
        unsigned tabc = msgSpan.count('\t');
        os.indent((info.at - msgSpan.data()) + (tabc * 3))
            .changeColor(inr::col::GREEN, true);
        os << '^';

        for(uint32_t i = 1; i < info.len; i++) {
            os << '~';
        }

        os.resetColor();

        os << '\n';
    }

    if(warnings || errors) {
        if(warnings) {
            os << warnings << " warnings";
        }
        if(errors) {
            if(warnings) os << " and ";
            os << errors << " errors";
        }
        os << " generated.\n";
    }
}

} // namespace inrcc