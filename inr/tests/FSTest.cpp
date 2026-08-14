// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Vfs/FStream.h>
#include <inr/Vfs/Vfs.h>

#include <cstring>
#include <system_error>

int main() {
    inr::vfs::Filesystem& fs = inr::vfs::getNativeFs();

    std::error_code ec;
    auto f =
        fs.open("file.txt",
                inr::vfs::OpenMode(inr::vfs::OWRITE | inr::vfs::OTRUNC), ec);

    if(ec != std::error_code() || !f) {
        inr::err() << ec.message() << '\n';
        return 1;
    }

    // Also test vfs stream
    inr::vfsstream fos(std::move(f), 0);
    fos << 42;

    f = fs.open("file.txt", inr::vfs::OREAD, ec);

    if(ec != std::error_code() || !f) {
        inr::err() << ec.message() << '\n';
        return 1;
    }

    char buff[2];
    f->read(buff, 2);

    if(std::memcmp(buff, "42", 2) != 0) {
        inr::err() << "Expected file contents to be 42\n";
        return 1;
    }

    (inr::out()
     << "File was successfully opened, wrote 42, and read 42\nFile contents: ")
            .write(buff, 2)
        << '\n';
    return 0;
}
