#ifndef INERTIA_IRKEYWORDS_HPP
#define INERTIA_IRKEYWORDS_HPP

#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace Inertia{
    enum class IRKeyword{
        NONE,
        #define ADD_KEYWORD(str, name) name,
        #include "Inertia/IR/IRKeywords.def"
        #undef ADD_KEYWORD
        ENDOFKEYWORDS,
    };
    
    inline const std::unordered_map<std::string_view, uint32_t> DefaultIRKeywordMap = {
        #define ADD_KEYWORD(str, name) {str, static_cast<uint32_t>(IRKeyword::name)},
        #include "Inertia/IR/IRKeywords.def"
        #undef ADD_KEYWORD
    };

    inline const std::unordered_map<std::string_view, uint32_t>& CreateIRKeywordMap(){
        return DefaultIRKeywordMap;
    }
}

#endif // INERTIA_IRKEYWORDS_HPP
