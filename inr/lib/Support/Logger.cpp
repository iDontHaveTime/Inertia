#include <inr/ADT/StrView.h>
#include <inr/Support/Logger.h>
#include <inr/Support/Stream.h>

#include <cstdint>

namespace inr::log {

static inline void printAuthor(raw_stream& os, sview author) {
    os << author << ": ";
}

static inline void printFile(raw_stream& os, sview file, uint32_t line,
                             uint32_t column) {
    os << file << ':' << line << ':' << column << ": ";
}

static inline void printLevel(raw_stream& os, Level lvl) {
    switch(lvl) {
        case Level::NONE:
            break;
        case Level::WARN:
            os.changeColor(WARN_COLOR, true);
            os << WARN_MSG << ": ";
            os.resetColor();
            break;
        case Level::ERROR:
            os.changeColor(ERROR_COLOR, true);
            os << ERROR_MSG << ": ";
            os.resetColor();
            break;
        default:
            break;
    }
}

void sendNoMsg(raw_stream& os, Level lvl, sview author) {
    if(!author.empty()) printAuthor(os, author);

    printLevel(os, lvl);
}

void send(raw_stream& os, Level lvl, sview author, sview msg) {
    sendNoMsg(os, lvl, author);

    os << msg << '\n';
}

void sendPosNoMsg(raw_stream& os, Level lvl, sview file, uint32_t line,
                  uint32_t column) {
    if(!file.empty()) printFile(os, file, line, column);

    printLevel(os, lvl);
}

void sendpos(raw_stream& os, Level lvl, sview file, uint32_t line,
             uint32_t column, sview msg) {
    sendPosNoMsg(os, lvl, file, line, column);

    os << msg << '\n';
}

} // namespace inr::log