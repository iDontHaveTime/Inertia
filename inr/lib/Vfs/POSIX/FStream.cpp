// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <inr/Vfs/FStream.h>
#include <inr/Vfs/Vfs.h>
#include <unistd.h>

#include <cstdlib>

namespace inr {

struct CheckFDColor {
    bool res;
    CheckFDColor(int fd) : res(false) {
        char* force_color = getenv("FORCE_COLOR");
        char* no_color = getenv("NO_COLOR");
        char* term = getenv("TERM");

        if(force_color != NULL && strcmp(force_color, "0") != 0) {
            res = true;
            return;
        }

        if(no_color != NULL) {
            return;
        }

        if(!isatty(fd)) {
            return;
        }

        if(term != NULL && strcmp(term, "dumb") == 0) {
            return;
        }

        res = true;
    }
};

class terminal_posix_vfsstream_stdout : public vfsstream {
public:
    using vfsstream::vfsstream;

    bool hasColors() const override {
        if(global_color_override == ColorOverride::AUTO) {
            static CheckFDColor colors(STDOUT_FILENO);
            return colors.res;
        }
        return global_color_override == ColorOverride::ALWAYS;
    }

    bool isDisplayed() const override {
        static bool disp = isatty(STDOUT_FILENO);
        return disp;
    }
};

class terminal_posix_vfsstream_stderr : public vfsstream {
public:
    using vfsstream::vfsstream;

    bool hasColors() const override {
        if(global_color_override == ColorOverride::AUTO) {
            static CheckFDColor colors(STDERR_FILENO);
            return colors.res;
        }
        return global_color_override == ColorOverride::ALWAYS;
    }

    bool isDisplayed() const override {
        static bool disp = isatty(STDERR_FILENO);
        return disp;
    }
};

stream& out() {
    static terminal_posix_vfsstream_stdout stdout_s(vfs::getStdout());
    return stdout_s;
}

stream& err() {
    static terminal_posix_vfsstream_stderr stderr_s(vfs::getStderr(), 0);
    return stderr_s;
}

stream& log() {
    static terminal_posix_vfsstream_stderr log_s(vfs::getStderr());
    return log_s;
}

} // namespace inr
