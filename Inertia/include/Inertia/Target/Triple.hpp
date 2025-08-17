#ifndef INERTIA_TRIPLE_HPP
#define INERTIA_TRIPLE_HPP

#include <cstdint>
#include <string_view>

namespace Inertia{
    enum class TargetType : uint16_t{
        None, x86, AArch64
    };
    enum class OSType : uint16_t{
        None, Linux
    };
    enum class EnvironmentType : uint16_t{
        None, GNU
    };
    enum class FileType : uint16_t{
        None, ELF, MachO, COFF
    };
    class TargetTriple{
        TargetType targetType;
        OSType osType;
        EnvironmentType envType;
        FileType fileType;
    public:

        TargetTriple() noexcept = default;

        TargetType getLoadedType(void) const noexcept{
            return targetType;
        }

        OSType getLoadedOS(void) const noexcept{
            return osType;
        }

        EnvironmentType getLoadedEnv(void) const noexcept{
            return envType;
        }

        FileType getFileType(void) const noexcept{
            return fileType;
        }

        constexpr TargetType str_to_target(const std::string_view& str) noexcept{
            constexpr std::pair<std::string_view, TargetType> table[] = {
                {"x86_64", TargetType::x86}, {"aarch64", TargetType::AArch64}
            };
            for(const auto& tbe : table){
                if(tbe.first == str){
                    return tbe.second;
                }
            }
            return TargetType::None;
        }

        constexpr OSType str_to_os(const std::string_view& str) noexcept{
            constexpr std::pair<std::string_view, OSType> table[] = {
                {"linux", OSType::Linux}
            };
            for(const auto& tbe : table){
                if(tbe.first == str){
                    return tbe.second;
                }
            }
            return OSType::None;
        }

        constexpr EnvironmentType str_to_env(const std::string_view& str) noexcept{
            constexpr std::pair<std::string_view, EnvironmentType> table[] = {
                {"gnu", EnvironmentType::GNU}
            };
            for(const auto& tbe : table){
                if(tbe.first == str){
                    return tbe.second;
                }
            }
            return EnvironmentType::None;
        }

        constexpr FileType os_to_filetype(OSType ost) noexcept{
            switch(ost){
                case OSType::None:
                    [[fallthrough]];
                case OSType::Linux:
                    return FileType::ELF;
            }
        }

        void load_target(const std::string_view& sw){
            size_t first_dash = sw.find('-');
            size_t second_dash = sw.find('-', first_dash + 1);

            std::string_view target = (first_dash == std::string_view::npos) ? sw : sw.substr(0, first_dash);
            std::string_view os = (first_dash == std::string_view::npos || second_dash == std::string_view::npos) ? std::string_view{} : sw.substr(first_dash + 1, second_dash - first_dash - 1);
            std::string_view env = (second_dash == std::string_view::npos) ? std::string_view{} : sw.substr(second_dash + 1);

            targetType = str_to_target(target);
            osType = str_to_os(os);
            envType = str_to_env(env);
            fileType = os_to_filetype(osType);
        }

        // format is similar to llvm's, basically <arch>-<os>-<env>
        TargetTriple(const std::string_view& sw){
            load_target(sw);
        }
    };
}

#endif // INERTIA_TRIPLE_HPP
