#ifndef INERTIA_TARGETFILE_HPP
#define INERTIA_TARGETFILE_HPP

#include "Inertia/Lexer/LexerFile.hpp"
#include <filesystem>

namespace Inertia{
    struct TargetFile{
        const LexerFile* lfile = nullptr;

        TargetFile(const LexerFile* file) noexcept : lfile(file){};

        const std::filesystem::path& get_path() const noexcept{
            return lfile->get_path();
        }

        operator bool() const noexcept{
            return lfile != nullptr;
        }
    };
}

#endif // INERTIA_TARGETFILE_HPP
