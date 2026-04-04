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

/// @brief Base class for inr-gen backends.
class CppEmitter {
protected:
    raw_stream& os_;

    template<typename... Args>
    void write(Args&&... args) {
        ((os_ << args), ...);
    }

    template<typename... Args>
    void writeln(Args&&... args) {
        write(std::forward<Args>(args)..., '\n');
    }

    template<typename... Args>
    void addComment(Args&&... args) {
        writeln("// ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    void addIfDef(Args&&... args) {
        writeln("#ifdef ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    void addIfNDef(Args&&... args) {
        writeln("#ifndef ", std::forward<Args>(args)...);
    }

    template<typename... Args>
    void addUndef(Args&&... args) {
        writeln("#undef ", std::forward<Args>(args)...);
    }

    void addEndIf() {
        writeln("#endif");
    }

    void openBody() {
        writeln('{');
    }

    void closeBody(bool semicolon = false) {
        writeln(semicolon ? "};" : "}");
    }

    void addNamespace(sview name) {
        write("namespace inr::", name);
    }

public:
    CppEmitter(raw_stream& os) noexcept : os_(os) {}

    raw_stream& getStream() noexcept {
        return os_;
    }

    /// @brief Emit C++ from the provided records.
    /// @param result Records.
    /// @return True on error, false on success.
    virtual bool emit(const RecordStorage& result) = 0;

    virtual ~CppEmitter() noexcept = default;
};

class RegisterBackend : public CppEmitter {
public:
    using CppEmitter::CppEmitter;

    bool emit(const RecordStorage& result) override;
};

class CallingConvBackend : public CppEmitter {
public:
    using CppEmitter::CppEmitter;

    bool emit(const RecordStorage& result) override;
};

class ISelBackend : public CppEmitter {
public:
    using CppEmitter::CppEmitter;

    bool emit(const RecordStorage& result) override;
};

} // namespace inr::gen

#endif // INERTIA_GEN_CPPEMITTER_H
