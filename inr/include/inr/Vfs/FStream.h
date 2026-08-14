// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_VFS_FSTREAM_H
#define INERTIA_VFS_FSTREAM_H

/// @file Vfs/FStream.h
/// @brief Provides a stream to a vfs file.

#include <inr/Support/Assert.h>
#include <inr/Support/Stream.h>
#include <inr/Vfs/Vfs.h>

#include <memory>

namespace inr {

/// @brief Provides a stream interface into a vfs file.
class vfsstream : public stream {
    std::unique_ptr<vfs::File> file_;

    void writeImpl(cbuff_t ptr, size_type size) override {
        inr_assert(file_.get() != nullptr, "File handle is nullptr");
        file_->write(ptr, size);
    }

public:
    vfsstream(std::unique_ptr<vfs::File> file,
              size_t bufferSize = DEFAULT_BUFFER_SIZE) :
        stream(bufferSize), file_(std::move(file)) {}

    ~vfsstream() override {
        setUnbuffered();
    }
};

} // namespace inr

#endif // INERTIA_VFS_FSTREAM_H
