// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INRCC_DRIVER_DRIVERFILEMANAGER_H
#define INRCC_DRIVER_DRIVERFILEMANAGER_H

/// @file Driver/DriverFileManager.h
/// @brief Provides the driver file manager class.

#include <inr/ADT/IVector.h>
#include <inr/ADT/StrView.h>
#include <inrcc/Support/FileManager.h>

#include <filesystem>

namespace inrcc {
// TODO: Add vfs-like system.

/// @brief Supports include directories unlike the regular file manager.
class DriverFMan {
public:
    struct File {
        const FileManager::File* file = nullptr;
        inr::sview originalName;
        size_t foundIndex = 0; ///< Used internally for include_next.
    };

private:
    FileManager fman_;
    std::string sysroot_;
    inr::ivec<std::string, 8> includePaths_;

public:
    DriverFMan() noexcept = default;

    const File wasOpenedBefore(inr::sview name) const noexcept {
        return {fman_.wasOpenedBefore(name), name, 0};
    }

    /// @brief Alias to FileManager::openFileBuffer().
    File openFileBufferNoInclude(inr::sview name) {
        return {fman_.openFileBuffer(name), name, 0};
    }

    bool isAbsolute(inr::sview p) {
        return std::filesystem::path(p.str()).is_absolute();
    }

    bool hasInclude(inr::sview name, size_t startAt = 0) {
        for(size_t i = startAt; i < includePaths_.size(); i++) {
            std::string tryFor = includePaths_[i];
            tryFor += name;
            if(fman_.exists(tryFor)) return true;
        }
        return false;
    }

    /// @brief Specifically for <...> includes.
    File openFileBufferNoLocalDir(inr::sview name, size_t startAt = 0) {
        if(isAbsolute(name)) return openFileBufferNoInclude(name);
        for(size_t i = startAt; i < includePaths_.size(); i++) {
            std::string tryFor = includePaths_[i];
            tryFor += name;
            const FileManager::File* f = fman_.openFileBuffer(tryFor);
            if(f) {
                return {f, name, i};
            }
        }
        return {nullptr, {}, 0};
    }

    /// @brief Specifically for "..." includes.
    File openFileBufferYesLocalDir(File file, inr::sview name,
                                   size_t startAt = 0) {
        if(isAbsolute(name)) return openFileBufferNoInclude(name);
        if(file.file && !startAt) {
            std::filesystem::path searchP = file.file->path;
            searchP.remove_filename();
            searchP /= std::string_view(name);
            if(const FileManager::File* f =
                   fman_.openFileBuffer(searchP.c_str())) {
                return {f, name, 0};
            }
        }
        return openFileBufferNoLocalDir(name, startAt);
    }

    bool setSysroot(std::string sysroot) noexcept {
        if(std::filesystem::is_directory(sysroot)) {
            sysroot_ = std::move(sysroot);
            char back = sysroot_.back();

            if(back == '/' || back == '\\') {
                sysroot_.pop_back();
            }

            return true;
        }
        return false;
    }

    /// @brief The driver is responsible for correct include ordering.
    /// @note This uses std::string not inr::sview so std::move() is
    /// recommended.
    void addIncludePath(std::string path) {
        char back = path.back();

        if(back != '/' && back != '\\') {
            path += '/';
        }
        includePaths_.emplace_back(std::move(path));
    }

    void addIncludePathRelativeToSysroot(std::string path) {
        std::string pathToAdd = sysroot_;
        char front = path.front();
        if(front != '/' && front != '\\') {
            pathToAdd += '/';
        }
        pathToAdd += path;
        char back = pathToAdd.back();

        if(back != '/' && back != '\\') {
            pathToAdd += '/';
        }
        includePaths_.emplace_back(std::move(pathToAdd));
    }

    void addLinuxLikeIncludePaths() {
        addIncludePathRelativeToSysroot("/usr/local/include/");
        addIncludePathRelativeToSysroot("/usr/include/");
    }
};

} // namespace inrcc

#endif // INRCC_DRIVER_DRIVERFILEMANAGER_H
