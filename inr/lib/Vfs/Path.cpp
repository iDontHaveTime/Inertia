// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Unreachable.h>
#include <inr/Vfs/Path.h>

#include <cctype>
#include <string>
#include <string_view>
#include <vector>

namespace inr::vfs {

#ifdef _WIN32
constexpr PathStyle native_path = PathStyle::windows;
#else
constexpr PathStyle native_path = PathStyle::posix;
#endif

constexpr std::size_t posix_path_max = 4096;
constexpr std::size_t posix_name_max = 255;

constexpr std::size_t win_path_max = 32767;
constexpr std::size_t win_name_max = 255;

static inline bool valid_posix(std::string_view path) {
    if(path.empty() || path.size() >= posix_path_max) return false;

    std::size_t start = 0;
    while(start < path.size()) {
        std::size_t end = path.find('/', start);
        std::size_t len = (end == std::string_view::npos)
                              ? (path.size() - start)
                              : (end - start);

        if(len >= posix_name_max) return false;
        if(end == std::string_view::npos) break;
        start = end + 1;
    }

    return true;
}

class WindowsChars {
    bool buff[256]{};

public:
    constexpr WindowsChars() {
        for(std::size_t i = 0; i < sizeof(buff); i++) {
            if(i == '<' || i == '>' || i == ':' || i == '"' || i == '|' ||
               i == '?' || i == '*') {
                buff[i] = false;
            }
            else {
                buff[i] = true;
            }
        }
    }

    constexpr bool allowed(char c) const {
        return buff[(unsigned char)c];
    }
};

constexpr static WindowsChars win_chars;

static inline bool valid_windows(std::string_view path) {
    if(path.empty()) return false;
    if(path.size() >= win_path_max) return false;

    std::size_t start = 0;

    if(path.size() >= 2) {
        if(path[1] == ':') {
            start = 2;
        }
    }

    const char* last_slash = path.data() + start;

    while(start < path.size()) {
        char c = path[start];

        if(c == '\\' || c == '/') {
            const char* cur_slash = path.data() + start;

            if(std::size_t(cur_slash - last_slash) >= win_name_max)
                return false;

            last_slash = cur_slash + 1;
            start++;
            continue;
        }

        if(!win_chars.allowed(c)) return false;

        start++;
    }

    const char* end_ptr = path.data() + path.size();
    if(std::size_t(end_ptr - last_slash) >= win_name_max) return false;

    return true;
}

bool valid(std::string_view path, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
            [[fallthrough]];
        case PathStyle::windows_slash:
            return valid_posix(path);
        case PathStyle::windows:
            return valid_windows(path);
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

static inline bool absolute_posix(std::string_view path) {
    return (path.size() != 0) ? (path[0] == '/') : false;
}

static inline bool absolute_windows(std::string_view path) {
    return (path.size() >= 2)
               ? ((((path[0] == '\\' || path[0] == '/') &&
                    (path[1] == '/' || path[1] == '\\'))
                       ? true
                       : ((path.size() >= 3)
                              ? (std::isalpha((unsigned char)path[0]) &&
                                 path[1] == ':' &&
                                 (path[2] == '/' || path[2] == '\\'))
                              : false)))
               : false;
}

bool absolute(std::string_view path, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
            [[fallthrough]];
        case PathStyle::windows_slash:
            return absolute_posix(path);
        case PathStyle::windows:
            return absolute_windows(path);
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

static inline void join_posix(std::string& lhs, std::string_view rhs) {
    if(!lhs.empty()) {
        if(lhs.back() != '/') {
            lhs += '/';
        }
    }
    lhs += rhs;
}

static inline void join_windows(std::string& lhs, std::string_view rhs) {
    if(!lhs.empty()) {
        if(lhs.back() != '/' && lhs.back() != '\\') {
            lhs += '\\';
        }
    }
    lhs += rhs;
}

void join(std::string& lhs, std::string_view rhs, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
        case PathStyle::windows_slash:
            join_posix(lhs, rhs);
            break;
        case PathStyle::windows:
            join_windows(lhs, rhs);
            break;
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

std::string join(std::string_view lhs, std::string_view rhs, PathStyle style) {
    std::string str(lhs);
    join(str, rhs, style);
    return str;
}

static inline std::string normalize_posix(std::string_view path) {
    if(path.empty()) return {};

    bool abs = absolute_posix(path);

    std::vector<std::string_view> components;
    std::size_t start = 0;

    while(start < path.size()) {
        std::size_t end = path.find('/', start);
        std::size_t len = (end == std::string_view::npos)
                              ? (path.size() - start)
                              : (end - start);

        if(len) {
            std::string_view comp = path.substr(start, len);
            if(comp == "..") {
                if(!components.empty() && components.back() != "..") {
                    components.pop_back();
                }
                else if(!abs) {
                    components.emplace_back(comp);
                }
            }
            else if(comp != ".") {
                components.emplace_back(comp);
            }
        }
        if(end == std::string_view::npos) break;
        start = end + 1;
    }

    std::string res = abs ? "/" : "";

    for(std::size_t i = 0; i < components.size(); ++i) {
        res.append(components[i].data(), components[i].size());
        if(i + 1 < components.size()) res += '/';
    }

    return res.empty() ? "." : res;
}

static inline std::string normalize_windows(std::string_view path) {
    if(path.empty()) return {};

    bool abs = absolute_windows(path);

    std::vector<std::string_view> components;
    std::size_t start = 0;
    std::size_t drive_prefix_len = 0;

    if(path.size() >= 2 && path[1] == ':') {
        drive_prefix_len =
            (path.size() >= 3 && (path[2] == '/' || path[2] == '\\')) ? 3 : 2;
        start = drive_prefix_len;
    }

    while(start < path.size()) {
        std::size_t end = std::string_view::npos;

        for(std::size_t i = start; i < path.size(); ++i) {
            if(path[i] == '/' || path[i] == '\\') {
                end = i;
                break;
            }
        }

        std::size_t len = (end == std::string_view::npos)
                              ? (path.size() - start)
                              : (end - start);

        if(len > 0) {
            std::string_view comp = path.substr(start, len);
            if(comp == "..") {
                if(!components.empty() && components.back() != "..") {
                    components.pop_back();
                }
                else if(!abs) {
                    components.emplace_back(comp);
                }
            }
            else if(comp != ".") {
                components.emplace_back(comp);
            }
        }
        if(end == std::string_view::npos) break;
        start = end + 1;
    }

    std::string result;
    if(drive_prefix_len > 0) {
        result.append(path, drive_prefix_len);
        if(drive_prefix_len == 3) result[2] = '\\';
    }
    else if(abs) {
        result += '\\';
    }

    for(std::size_t i = 0; i < components.size(); ++i) {
        result.append(components[i].data(), components[i].size());
        if(i + 1 < components.size()) result += '\\';
    }

    return result.empty() ? "." : result;
}

std::string normalize(std::string_view path, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
        case PathStyle::windows_slash:
            return normalize_posix(path);
        case PathStyle::windows:
            return normalize_windows(path);
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

static inline std::string_view parent_posix(std::string_view path) {
    if(path.empty()) return {};

    std::size_t end = path.size();
    if(end > 1 && path[end - 1] == '/') {
        end--;
    }

    std::size_t last_slash = path.substr(0, end).find_last_of('/');
    if(last_slash == std::string_view::npos) {
        return (path[0] == '/') ? path.substr(0, 1) : std::string_view{};
    }

    if(last_slash == 0) return path.substr(0, 1);
    return path.substr(0, last_slash);
}

static inline std::string_view parent_windows(std::string_view path) {
    if(path.empty()) return {};

    std::size_t end = path.size();
    if(end > 1 && (path[end - 1] == '/' || path[end - 1] == '\\')) {
        end--;
    }

    std::string_view sub = path.substr(0, end);

    std::size_t last_slash = sub.find_last_of('/');
    std::size_t from_offset =
        (last_slash == std::string_view::npos) ? 0 : last_slash + 1;
    std::size_t backslash_pos = sub.find_last_of('\\', from_offset);

    if(backslash_pos != std::string_view::npos) {
        last_slash = backslash_pos;
    }

    if(last_slash == 2 && path[1] == ':') {
        return path.substr(0, 3);
    }

    if(last_slash == std::string_view::npos) {
        if(path.size() >= 2 && path[1] == ':') return path.substr(0, 2);
        return {};
    }

    if(last_slash == 0) return path.substr(0, 1);

    return path.substr(0, last_slash);
}

std::string_view parent(std::string_view path, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
        case PathStyle::windows_slash:
            return parent_posix(path);
        case PathStyle::windows:
            return parent_windows(path);
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

static inline std::string_view filename_posix(std::string_view path) {
    std::size_t last = path.find_last_of('/');
    if(last == std::string_view::npos) return path;

    if(path.size() - 1 == last) return {};

    return path.substr(last + 1, std::string_view::npos);
}

static inline std::string_view filename_windows(std::string_view path) {
    std::size_t last = path.find_last_of('/');

    std::size_t from_offset = (last == std::string_view::npos) ? 0 : last + 1;
    std::size_t backslash_pos = path.find_last_of('\\', from_offset);

    if(backslash_pos != std::string_view::npos) {
        last = backslash_pos;
    }

    if(last == std::string_view::npos) {
        if(path.size() == 2 && path[1] == ':') return {};
        return path;
    }

    if(path.size() - 1 == last) return {};
    return path.substr(last + 1, std::string_view::npos);
}

std::string_view filename(std::string_view path, PathStyle style) {
    if(style == PathStyle::native) style = native_path;
    switch(style) {
        case PathStyle::posix:
        case PathStyle::windows_slash:
            return filename_posix(path);
        case PathStyle::windows:
            return filename_windows(path);
        default:
            inr_unreachable("Native path is set beforehand");
    }
}

std::string_view extension(std::string_view path, PathStyle style) {
    std::string_view fname = filename(path, style);
    if(fname.empty()) return {};
    if(std::size_t pos = fname.find_last_of('.');
       pos != std::string_view::npos && pos != 0) {
        return fname.substr(pos, std::string_view::npos);
    }
    return {};
}

std::string_view stem(std::string_view path, PathStyle style) {
    std::string_view fname = filename(path, style);
    if(fname.empty()) return {};

    if(std::size_t pos = fname.find_last_of('.');
       pos != std::string_view::npos && pos != 0) {
        fname = fname.substr(0, pos);
    }

    return fname;
}

bool hasExtension(std::string_view path, PathStyle style) {
    return !extension(path, style).empty();
}

} // namespace inr::vfs
