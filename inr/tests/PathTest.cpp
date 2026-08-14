// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <inr/Vfs/Path.h>

#include <string_view>

int main() {
    constexpr std::string_view test_path = "/usr/lib/libc.so";
    constexpr inr::vfs::PathStyle style = inr::vfs::PathStyle::posix;

    if(!inr::vfs::absolute(test_path, style)) {
        inr::err() << "Path is absolute\n";
        return 1;
    }

    if(auto ext = inr::vfs::extension(test_path, style); ext != ".so") {
        inr::err() << "Extension should be '.so' but got '" << ext << "'\n";
        return 1;
    }

    if(auto fname = inr::vfs::filename(test_path, style); fname != "libc.so") {
        inr::err() << "Filename should be 'libc.so' but got '" << fname
                   << "'\n";
        return 1;
    }

    if(auto stem = inr::vfs::stem(test_path, style); stem != "libc") {
        inr::err() << "Stem should be 'libc' but got '" << stem << "'\n";
        return 1;
    }

    if(auto parent = inr::vfs::parent(test_path, style); parent != "/usr/lib") {
        inr::err() << "Parent should be '/usr/lib' but got '" << parent
                   << "'\n";
        return 1;
    }

    return 0;
}
