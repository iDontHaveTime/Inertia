// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_SUPPORT_FILEMANAGER_H
#define INRCC_SUPPORT_FILEMANAGER_H

/// @file Support/FileManager.h
/// @brief Manages files and their buffers.

#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inr/Support/CFile.h>
#include <inr/Support/MemoryFile.h>
#include <inrcc/ADT/StringMap.h>

#include <filesystem>

namespace inrcc {

/// @brief Provides a way to unique file buffers.
class FileManager {
public:
    struct File {
        inr::CFile cfile;
        inr::MemoryFile memfile;
        std::filesystem::path path;

        ~File() noexcept = default;

    private:
        File(inr::CFile cf, inr::MemoryFile memf, std::filesystem::path fpath) :
            cfile(std::move(cf)),
            memfile(std::move(memf)),
            path(std::move(fpath)) {}

        friend class FileManager;
    };

private:
    inr::ivec<File*, 4> files_;
    StringMap<size_t> cache_;

public:
    File* newCustomFile(char* start, char* end) {
        return files_.emplace_back(new File({}, {start, end}, {}));
    }

    bool exists(inr::sview fileName) {
        return std::filesystem::exists(fileName.strv());
    }

    const File* openFileBuffer(inr::sview fileName) {
        if(const size_t* idx = cache_.find(fileName)) {
            return files_[*idx];
        }

        std::error_code ec;
        std::filesystem::path canon =
            std::filesystem::canonical(fileName.str(), ec);

        if(ec) return nullptr;

        size_t canonPathLen = inr::str::length(canon.c_str());

        auto it = cache_.find(canon.c_str(), canonPathLen);
        if(it) {
            return files_[*it];
        }

        inr::CFile cfile(canon.c_str(), "rb");
        inr::MemoryFile memf(cfile, cfile.readFileSizeFromStart(), true, '\0');
        File* newFile =
            new File(std::move(cfile), std::move(memf), std::move(canon));

        size_t atIdx = files_.size();
        files_.emplace_back(newFile);

        cache_.insert(newFile->path.c_str(), canonPathLen, atIdx);
        cache_.insert(fileName, atIdx);

        return newFile;
    }

    const File* wasOpenedBefore(inr::sview name) const noexcept {
        const size_t* ff = cache_.find(name);
        return ff ? files_[*ff] : nullptr;
    }
};

} // namespace inrcc

#endif // INRCC_SUPPORT_FILEMANAGER_H
