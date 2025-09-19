#ifndef INRCC_LINKER_HPP
#define INRCC_LINKER_HPP

#include <string_view>

namespace inrcc{
    enum class LinkerStyle{
        GNU, MSVC, MACOS
    };
    constexpr const std::string_view enum_str(LinkerStyle style) noexcept{
        switch(style){
            case LinkerStyle::GNU:
                return "gnu";
            case LinkerStyle::MSVC:
                return "msvc";
            case LinkerStyle::MACOS:
                return "macos";
            default:
                return "";
        }
    }
    enum class LinkerFlavor{
        LD, LLD, GOLD, MOLD, MSVC_LINK, CUSTOM
    };
    constexpr const std::string_view enum_str(LinkerFlavor flavor, LinkerStyle style) noexcept{
        switch(flavor){
            case LinkerFlavor::LD:
                return "ld";
            case LinkerFlavor::LLD:
                switch(style){
                    case LinkerStyle::GNU:
                        return "ld.lld";
                    case LinkerStyle::MSVC:
                        return "ld-link";
                    case LinkerStyle::MACOS:
                        return "ld64.lld";
                    default:
                        return "ld.lld";
                }
            case LinkerFlavor::GOLD:
                return "ld.gold";
            case LinkerFlavor::MOLD:
                return "ld.mold";
            case LinkerFlavor::MSVC_LINK:
                return "link.exe";
            case LinkerFlavor::CUSTOM:
                [[fallthrough]];
            default:
                return "custom";
        }
    }
    class Linker{
        std::string_view command;
        LinkerStyle style;
        LinkerFlavor flavor;

    public:

        constexpr const std::string_view& getName(void) const noexcept{
            return command;
        }

        constexpr void setStyle(LinkerStyle _style) noexcept{
            style = _style;
        }

        constexpr LinkerStyle getStyle(void) const noexcept{
            return style;
        }

        constexpr void setFlavor(LinkerFlavor _flavor) noexcept{
            flavor = _flavor;
        }

        constexpr LinkerFlavor getFlavor(void) const noexcept{
            return flavor;
        }

        constexpr void setCustomLinker(const std::string_view& name, LinkerStyle _style) noexcept{
            flavor = LinkerFlavor::CUSTOM;
            command = name;
            style = _style;
        }

        constexpr Linker() noexcept = default;
        constexpr Linker(LinkerFlavor _flavor, LinkerStyle _style, const std::string_view& name = "") noexcept : style(_style), flavor(_flavor){
            if(_flavor == LinkerFlavor::CUSTOM){
                command = name;
            }
            else{
                command = enum_str(_flavor, _style);
            }
        };

        friend class LinkerBuilder;
    };
    namespace linkers{
        constexpr const Linker ld_linker = {LinkerFlavor::LD, LinkerStyle::GNU};
        constexpr const Linker ld_gold_linker = {LinkerFlavor::GOLD, LinkerStyle::GNU};
        constexpr const Linker ld_mold_linker = {LinkerFlavor::MOLD, LinkerStyle::GNU};
        constexpr const Linker lld_linker_gnu = {LinkerFlavor::LLD, LinkerStyle::GNU};
        constexpr const Linker lld_linker_ms = {LinkerFlavor::LLD, LinkerStyle::MSVC};
        constexpr const Linker lld_linker_mac = {LinkerFlavor::LLD, LinkerStyle::MACOS};
        constexpr const Linker msvc_linker = {LinkerFlavor::MSVC_LINK, LinkerStyle::MSVC};
    }
}

#endif // INRCC_LINKER_HPP
