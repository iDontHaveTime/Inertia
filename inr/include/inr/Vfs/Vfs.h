// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_VFS_VFS_H
#define INERTIA_VFS_VFS_H

/// @file Vfs/Vfs.h
/// @brief Provides an abstraction over the actual filesystem.

#include <inr/Vfs/Path.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace inr::vfs {

/// @brief How should the file be opened.
enum OpenMode : unsigned char {
    OREAD = 0b1,     ///< Allow reading from the file.
    OWRITE = 0b10,   ///< Allow writing to the file.
    OAPPEND = 0b100, ///< Start from the file's end.
    OTRUNC = 0b1000, ///< Erase file contents.
};

/// @brief Aliases for SEEK_* macros.
enum SeekType : unsigned char {
    SSET = SEEK_SET, ///< Alias for SEEK_SET.
    SCUR = SEEK_CUR, ///< Alias for SEEK_CUR.
    SEND = SEEK_END, ///< Alias for SEEK_END.
};

/// @brief Type of the underlying file.
enum class FileType : unsigned char {
    None,            ///< Nothing.
    NotFound,        ///< File not found.
    Regular,         ///< Regular file.
    Directory,       ///< Directory.
    Symlink,         ///< Symlink.
    CharacterDevice, ///< Character device.
    FIFO,            ///< FIFO.
    Socket,          ///< Socket.
};

/// @brief Base class for all files.
class File {
public:
    virtual ~File() = default;

    /// @brief Reads an amount of bytes into the buffer.
    virtual std::size_t read(void* buffer, std::size_t bytes) = 0;

    /// @brief Writes an amount of bytes into the file.
    virtual std::size_t write(const void* buffer, std::size_t bytes) = 0;

    /// @brief Changes the read/write pointer in the file.
    virtual bool seek(std::size_t offset, SeekType st) = 0;

    /// @brief Tells the current offset within the file.
    virtual std::size_t tell() const = 0;
};

/// @brief File permissions.
enum class Perms : uint16_t {
    None = 0000,
    UserRead = 0400,   // 0400
    UserWrite = 0200,  // 0200
    UserExe = 0100,    // 0100
    GroupRead = 0040,  // 0040
    GroupWrite = 0020, // 0020
    GroupExe = 0010,   // 0010
    OtherRead = 0004,  // 0004
    OtherWrite = 0002, // 0002
    OtherExe = 0001,   // 0001
    All = 0xFFF,
};

/// @brief Unique ID of the file.
class UniqueID {
    uint64_t device_;
    uint64_t inode_;

public:
    UniqueID(uint64_t device, uint64_t inode) :
        device_(device), inode_(inode) {}

    /// @brief Compares whether or not the files are the same.
    bool operator==(const UniqueID& other) const {
        return device_ == other.device_ && inode_ == other.inode_;
    }

    uint64_t getDevice() const {
        return device_;
    }

    uint64_t getINode() const {
        return inode_;
    }
};

/// @brief Returned by the stat command, contains info about the file.
class FSStat {
    std::string name_;
    UniqueID uid_{0, 0};
    uint64_t mtime_ = 0;
    uint32_t user_ = 0;
    uint32_t group_ = 0;
    uint64_t size_ = 0;
    FileType ft_ = FileType::None;
    Perms perms_ = Perms::None;

public:
    FSStat() = default;
    FSStat(std::string name, UniqueID uid, uint64_t mtime, uint32_t user,
           uint32_t group, uint64_t size, FileType ft, Perms perms) :
        name_(std::move(name)),
        uid_(uid),
        mtime_(mtime),
        user_(user),
        group_(group),
        size_(size),
        ft_(ft),
        perms_(perms) {}

    FSStat(FileType ft) : ft_(ft) {}

    /// @brief Returns the name of the file.
    const std::string& getName() const {
        return name_;
    }

    /// @brief Returns the unique id of the file.
    UniqueID getUID() const {
        return uid_;
    }

    /// @brief Returns file's modification time.
    uint64_t getModificationTime() const {
        return mtime_;
    }

    uint32_t getUser() const {
        return user_;
    }

    uint32_t getGroup() const {
        return group_;
    }

    /// @brief Returns file's size.
    uint64_t getSize() const {
        return size_;
    }

    /// @brief Returns the file's type.
    FileType getFT() const {
        return ft_;
    }

    /// @brief Returns file's permissions.
    Perms getPerms() const {
        return perms_;
    }
};

/// @brief Base class for all filesystem abstractions.
class Filesystem {
public:
    virtual ~Filesystem() = default;

    /// @brief Opens a file, returns nullptr on error.
    virtual std::unique_ptr<File> open(std::string_view path, OpenMode om,
                                       std::error_code& ec) = 0;

    /// @brief Removes a file from the filesystem.
    virtual std::error_code rm(std::string_view path) = 0;

    /// @brief Creates a new or multiple new directories.
    virtual std::error_code mkdir(std::string_view path) = 0;

    virtual std::vector<std::string> listDirs(std::string_view path,
                                              std::error_code& ec) = 0;

    virtual FSStat stat(std::string_view path, std::error_code& ec) = 0;

    bool exists(std::string_view path) {
        std::error_code ec;
        auto s = stat(path, ec);
        return !ec && s.getFT() != FileType::NotFound &&
               s.getFT() != FileType::None;
    }
};

/// @brief Returns the host's native fs.
Filesystem& getNativeFs();
/// @brief Returns a writable file handle to stdout.
std::unique_ptr<File> getStdout();
/// @brief Returns a writable file handle to stderr.
std::unique_ptr<File> getStderr();
/// @brief Returns a readable file handle to stdin.
std::unique_ptr<File> getStdin();

} // namespace inr::vfs

#endif // INERTIA_VFS_VFS_H
