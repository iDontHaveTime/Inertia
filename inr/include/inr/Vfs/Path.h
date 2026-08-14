// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_VFS_PATH_H
#define INERTIA_VFS_PATH_H

/// @file Vfs/Path.h
/// @brief Uses std::string as a path base class.

#include <string_view>

namespace inr::vfs {

enum class PathStyle {
    native,        ///< Depends on the host platform.
    posix,         ///< e.g. /foo/bar/baz
    windows,       ///< e.g. C:\\Foo\\Bar.baz
    windows_slash, ///< e.g. /c/Foo/Bar.baz
};

/// @brief Returns whether or not the path is valid to the style selected.
bool valid(std::string_view path, PathStyle style = PathStyle::native);

/// @brief Returns whether or not the provided path is absolute.
bool absolute(std::string_view path, PathStyle style = PathStyle::native);

/// @brief Joins two paths together using the style provided.
/// @note handles trailing slahes. Modifies the string in place.
void join(std::string& lhs, std::string_view rhs,
          PathStyle style = PathStyle::native);

/// @brief Joins two paths together using the style provided.
/// @note Handles trailing slashes.
std::string join(std::string_view lhs, std::string_view rhs,
                 PathStyle style = PathStyle::native);

/// @brief Resolves dots and separators.
/// @note Does not access the filesystem.
std::string normalize(std::string_view path,
                      PathStyle style = PathStyle::native);

/// @brief Returns the parent name.
std::string_view parent(std::string_view path,
                        PathStyle style = PathStyle::native);

/// @brief Returns the filename if present.
std::string_view filename(std::string_view path,
                          PathStyle style = PathStyle::native);

/// @brief Returns the file extension.
std::string_view extension(std::string_view path,
                           PathStyle style = PathStyle::native);

/// @brief Returns the filename without the extension.
std::string_view stem(std::string_view path,
                      PathStyle style = PathStyle::native);

/// @brief Returns whether or not the file has an extension.
bool hasExtension(std::string_view path, PathStyle style = PathStyle::native);

} // namespace inr::vfs

#endif // INERTIA_VFS_PATH_H
