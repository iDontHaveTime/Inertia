#ifndef INERTIA_SUPPORT_TERMINAL_H
#define INERTIA_SUPPORT_TERMINAL_H

/// @file Support/Terminal.h
/// @brief Provides terminal-related features.

#include <inr/Support/Stream.h>

namespace inr::terminal {

/// @brief Gets the width of the terminal if the stream is displayed.
/// @param s Stream connected to the terminal (usually outs()/errs()).
/// @return Width of the terminal, -1 if failed, 0 if not a terminal.
int getWidth(stream& s);

} // namespace inr::terminal

#endif // INERTIA_SUPPORT_TERMINAL_H
