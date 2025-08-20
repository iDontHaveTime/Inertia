#ifndef INERTIA_TRIPLE_HPP
#define INERTIA_TRIPLE_HPP

#include <cstdint>
#include <string_view>
#include <type_traits>

namespace Inertia{
    enum class TargetType : uint16_t{
        None, x86, AArch64
    };
    enum class OSType : uint16_t{
        None, Unknown, Linux
    };
    enum class EnvironmentType : uint16_t{
        None, GNU
    };
    enum class FileType : uint16_t{
        None, ELF, MachO, COFF
    };
    enum class ABIType : uint16_t{
        None, SystemV
    };
    class TargetTriple{
        std::string_view str;
        TargetType targetType;
        OSType osType;
        EnvironmentType envType;
        FileType fileType;
        ABIType abi;
    public:

        constexpr TargetTriple() noexcept = default;

        constexpr TargetType getLoadedType(void) const noexcept{
            return targetType;
        }

        constexpr OSType getLoadedOS(void) const noexcept{
            return osType;
        }

        constexpr EnvironmentType getLoadedEnv(void) const noexcept{
            return envType;
        }

        constexpr FileType getLoadedFileType(void) const noexcept{
            return getFileType();
        }
        constexpr FileType getFileType(void) const noexcept{
            return fileType;
        }

        constexpr ABIType getLoadedABI(void) const noexcept{
            return abi;
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
                {"linux", OSType::Linux},
                {"unknown", OSType::Unknown}
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

        constexpr FileType os_to_filetype(OSType ost) const noexcept{
            switch(ost){
                case OSType::None:
                    return FileType::None;
                case OSType::Unknown:
                    [[fallthrough]];
                case OSType::Linux:
                    return FileType::ELF;
            }
        }

        constexpr ABIType target_to_abi() const noexcept{
            switch(targetType){
                case TargetType::None:
                    return ABIType::None;
                case TargetType::x86:
                    switch(osType){
                        case OSType::None:
                            return ABIType::None;
                        case OSType::Unknown:
                            [[fallthrough]];
                        case OSType::Linux:
                            return ABIType::SystemV;
                    }
                    return ABIType::None;
                case TargetType::AArch64:
                    return ABIType::None;
            }
        }

        constexpr const std::string_view& getLoadedString() const noexcept{
            return str;
        }

        constexpr explicit operator bool() const noexcept{
            return (abi != ABIType::None && targetType != TargetType::None && osType != OSType::None);
        }

        constexpr bool isValid() const noexcept{
            return (bool)(*this);
        }

        constexpr void load_target(const std::string_view& sw){
            size_t first_dash = sw.find('-');
            size_t second_dash = sw.find('-', first_dash + 1);

            std::string_view target = (first_dash == std::string_view::npos) ? sw : sw.substr(0, first_dash);
            std::string_view os = (first_dash == std::string_view::npos || second_dash == std::string_view::npos) ? std::string_view{} : sw.substr(first_dash + 1, second_dash - first_dash - 1);
            std::string_view env = (second_dash == std::string_view::npos) ? std::string_view{} : sw.substr(second_dash + 1);

            str = sw;
            targetType = str_to_target(target);
            osType = str_to_os(os);
            envType = str_to_env(env);
            fileType = os_to_filetype(osType);
            abi = target_to_abi();

            if(std::is_constant_evaluated()){
                if(!isValid()){
                    throw "Invalid target triple string.";
                }
            }
        }

        // format is similar to llvm's, basically <arch>-<os>-<env>
        constexpr TargetTriple(const std::string_view& sw){
            load_target(sw);
        }
    };
}

#endif // INERTIA_TRIPLE_HPP
