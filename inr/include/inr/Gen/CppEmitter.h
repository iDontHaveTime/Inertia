// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_CPPEMITTER_H
#define INERTIA_GEN_CPPEMITTER_H

/// @file Gen/CppEmitter.h
/// @brief Emits C++ from the inr-gen files.

#include <inr/Gen/Record.h>
#include <inr/Support/Stream.h>

namespace inr::gen {

class emitter {
public:
    /// @brief Base class for inr-gen backends.
    class CppEmitter {
    protected:
        raw_stream& os_;

        template<typename... Args>
        void write(Args&&... args) {
            ((os_ << args), ...);
        }

    public:
        CppEmitter(raw_stream& os) noexcept : os_(os) {}

        raw_stream& getStream() noexcept {
            return os_;
        }

        /// @brief Emit C++ from the provided records.
        /// @param result Records.
        /// @return True on error, false on success.
        virtual bool emit(RecordStorage& result);
    };
};

} // namespace inr::gen

#endif // INERTIA_GEN_CPPEMITTER_H
