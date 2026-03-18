#ifndef INERTIA_SUPPORT_LOGGER_H
#define INERTIA_SUPPORT_LOGGER_H

/// @file Support/Logger.h
/// @brief Provides logging related functions.

#include <inr/ADT/StrView.h>
#include <inr/Support/Stream.h>

#include <cstdint>

namespace inr::log {

enum class Level : uint8_t {
    NONE, ///< Doesn't print anything extra.
    WARN, ///< Appends 'WARN_MSG' and a colon with a space after.
    ERROR ///< Appends 'ERROR_MSG' and a colon with a space after.
};

constexpr Colors WARN_COLOR = col::YELLOW;
constexpr Colors ERROR_COLOR = col::RED;

constexpr const char* ERROR_MSG = "error";
constexpr const char* WARN_MSG = "warning";

/// @brief Creates an empty string view, used to mark empty in
/// 'send(raw_stream&, Level, sview, sview)'.
/// @return The new string view.
constexpr sview none() noexcept {
    return sview();
}

/// @brief Sends a message to the stream provided.
/// @param os Stream to send it to.
/// @param lvl Level of the message (e.g. ERROR will display "error: ").
/// @param author Source of the message. If empty, no prefix is printed.
/// @param msg The message after the author and level.
///
/// For example doing `send(errs(), Level::WARN, "inertia", "test warning")`
/// would output: "inertia: warning: test warning"
void send(raw_stream& os, Level lvl, sview author, sview msg);

/// @brief Works exactly like send but without the message.
/// @see send(raw_stream&, Level, sview, sview) for more info.
void sendNoMsg(raw_stream& os, Level lvl, sview author);
/// @brief Same thing here but for sendpos.
/// @see sendPosNoMsg(raw_stream&, Level, sview, sview) for more info.
void sendPosNoMsg(raw_stream& os, Level lvl, sview file, uint32_t line,
                  uint32_t column);

/// @brief Sends a message to the stream provided, preferred for files.
/// @param os Stream to send it to.
/// @param lvl Level of the message.
/// @param file File from which the position is.
/// @param line What line is it from.
/// @param column What column is it from.
/// @param msg The message after the file, position, and level.
///
/// For example doing `sendpos(errs(), Level::WARN, "main.cpp", 3, 20, "test")`
/// would output: "main.cpp:3:20: warning: test"
void sendpos(raw_stream& os, Level lvl, sview file, uint32_t line,
             uint32_t column, sview msg);

} // namespace inr::log

#endif // INERTIA_SUPPORT_LOGGER_H
