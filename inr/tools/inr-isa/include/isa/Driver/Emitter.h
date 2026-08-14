// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_DRIVER_EMITTER
#define INERTIA_DRIVER_EMITTER

/// @file Driver/Emitter.h
/// @brief Emits C++ files from parsed .isa files.

#include <inr/ADT/ArrView.h>
#include <inr/Support/Stream.h>
#include <isa/Parser/Expression.h>

#include <string_view>

namespace isa {

class Emitter;

struct EmitterBackend {
    std::string_view name;
    const Emitter* emitter;
    std::string_view helptext;
};

class Emitter {
public:
    /// @brief Emits C++ from the parsed expressions vector.
    virtual bool emit(inr::stream&,
                      const std::vector<std::unique_ptr<Value>>&) const = 0;
    static const EmitterBackend* getByName(std::string_view view);
    static inr::arrview<EmitterBackend> getAvailableEmitters();
};

} // namespace isa

#endif // INERTIA_DRIVER_EMITTER
