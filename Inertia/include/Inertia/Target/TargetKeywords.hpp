#ifndef INERTIA_TARGETKEYWORDS_HPP
#define INERTIA_TARGETKEYWORDS_HPP

#include <unordered_map>
#include <string_view>

namespace Inertia{
    enum class TargetKeyword{
        NONE,
        #define ADD_KEYWORD(str, name) name,
        #include "Inertia/Target/TargetKeywords.def"
        #undef ADD_KEYWORD
        ENDOFKEYWORDS,
    };
    
    inline const std::unordered_map<std::string_view, uint32_t> DefaultTargetKeywordMap = {
        #define ADD_KEYWORD(str, name) {str, static_cast<uint32_t>(TargetKeyword::name)},
        #include "Inertia/Target/TargetKeywords.def"
        #undef ADD_KEYWORD
    };

    inline const std::unordered_map<std::string_view, uint32_t>& CreateTargetKeywordMap(){
        return DefaultTargetKeywordMap;
    }
}

#endif // INERTIA_TARGETKEYWORDS_HPP
