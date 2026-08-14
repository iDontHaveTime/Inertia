// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <dirent.h>
#include <fcntl.h>
#include <inr/Vfs/Vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstddef>
#include <memory>
#include <system_error>

namespace inr::vfs {

class BasePosixFile : public File {
protected:
    int fd_;

public:
    BasePosixFile(int fd) : fd_(fd) {}

    std::size_t read(void* buffer, std::size_t bytes) override {
        auto res = ::read(fd_, buffer, bytes);
        return (res < 0) ? 0 : std::size_t(res);
    }

    std::size_t write(const void* buffer, std::size_t bytes) override {
        ssize_t res = ::write(fd_, buffer, bytes);
        return (res < 0) ? 0 : std::size_t(res);
    }

    bool seek(std::size_t offset, SeekType st) override {
        return ::lseek(fd_, off_t(offset), st) != -1;
    }

    std::size_t tell() const override {
        off_t res = ::lseek(fd_, 0, SEEK_CUR);
        return (res == -1) ? 0 : std::size_t(res);
    }
};

class PosixFile : public BasePosixFile {
public:
    using BasePosixFile::BasePosixFile;

    ~PosixFile() override {
        if(fd_ != -1) ::close(fd_);
    }
};

class PosixFilesystem : public Filesystem {
public:
    std::unique_ptr<File> open(std::string_view path, OpenMode om,
                               std::error_code& ec) override {
        std::string path_str(path);

        int flags = 0;
        if((om & OREAD) && (om & OWRITE)) flags |= O_RDWR;
        else if(om & OWRITE) flags |= O_WRONLY;
        else if(om & OREAD) flags |= O_RDONLY;

        if(om & OWRITE) flags |= O_CREAT;
        if(om & OAPPEND) flags |= O_APPEND;
        if(om & OTRUNC) flags |= O_TRUNC;

        int fd = ::open(path_str.c_str(), flags, 0666);
        if(fd == -1) {
            ec = std::make_error_code(std::errc(errno));
            return nullptr;
        }
        return std::make_unique<PosixFile>(fd);
    }

    std::error_code rm(std::string_view path) override {
        std::string path_str(path);
        if(::unlink(path_str.c_str()) == -1) {
            return std::make_error_code(std::errc(errno));
        }
        return {};
    }

    std::error_code mkdir(std::string_view path) override {
        std::string path_str(path);
        if(::mkdir(path_str.c_str(), 0777) == -1) {
            if(errno != EEXIST) {
                return std::make_error_code(std::errc(errno));
            }
        }
        return {};
    }

    std::vector<std::string> listDirs(std::string_view path,
                                      std::error_code& ec) override {
        std::string path_str(path);
        DIR* dir = ::opendir(path_str.c_str());
        if(!dir) {
            ec = std::make_error_code(std::errc(errno));
            return {};
        }

        std::vector<std::string> results;
        while(struct dirent* entry = ::readdir(dir)) {
            std::string_view name(entry->d_name);
            if(name != "." && name != "..") {
                results.emplace_back(name);
            }
        }
        ::closedir(dir);
        return results;
    }

    FSStat stat(std::string_view path, std::error_code& ec) override {
        std::string path_str(path);
        struct ::stat st;

        if(::stat(path_str.c_str(), &st) == -1) {
            if(errno == ENOENT) return FSStat(FileType::NotFound);
            ec = std::make_error_code(std::errc(errno));
            return FSStat(FileType::None);
        }

        FileType ft = FileType::None;
        if(S_ISREG(st.st_mode)) ft = FileType::Regular;
        else if(S_ISDIR(st.st_mode)) ft = FileType::Directory;
        else if(S_ISLNK(st.st_mode)) ft = FileType::Symlink;
        else if(S_ISFIFO(st.st_mode)) ft = FileType::FIFO;
        else if(S_ISSOCK(st.st_mode)) ft = FileType::Socket;
        else if(S_ISCHR(st.st_mode)) ft = FileType::CharacterDevice;

        UniqueID uid{uint64_t(st.st_dev), uint64_t(st.st_ino)};
        Perms perms = Perms(st.st_mode & mode_t(Perms::All));

        std::string name(filename(path, PathStyle::posix));

        return FSStat(name, uid, uint64_t(st.st_mtime), uint32_t(st.st_uid),
                      uint32_t(st.st_gid), uint64_t(st.st_size), ft, perms);
    }
};

Filesystem& getNativeFs() {
    static PosixFilesystem fs;
    return fs;
}

std::unique_ptr<File> getStdout() {
    return std::make_unique<BasePosixFile>(STDOUT_FILENO);
}

std::unique_ptr<File> getStderr() {
    return std::make_unique<BasePosixFile>(STDERR_FILENO);
}

std::unique_ptr<File> getStdin() {
    return std::make_unique<BasePosixFile>(STDIN_FILENO);
}

} // namespace inr::vfs
