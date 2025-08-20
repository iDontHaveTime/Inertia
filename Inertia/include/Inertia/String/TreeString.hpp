#ifndef INERTIA_TREESTRING_HPP
#define INERTIA_TREESTRING_HPP

#include <ostream>

namespace Inertia{
    class TreeString{
        const char* str;
        TreeString* next;
    public:

        friend std::ostream& operator<<(std::ostream& lhs, const TreeString& rhs){
            if(!rhs.str) return lhs;
            lhs<<rhs.str;
            const TreeString* candidate = rhs.next;

            while(candidate){
                lhs<<candidate->str;
                candidate = candidate->next;
            }
            return lhs;
        }

        constexpr const char* data() const noexcept{
            return str;
        }

        constexpr const std::string_view view() const noexcept{
            return std::string_view(str);
        }

        constexpr TreeString* getNext(void) noexcept{
            return next;
        }
        constexpr const TreeString* getNext(void) const noexcept{
            return next;
        }

        constexpr void setNext(TreeString* _next) noexcept{
            next = _next;
        }

        constexpr TreeString(const char* _str) noexcept : str(_str), next(nullptr){};
        constexpr TreeString(const std::string_view& _str) noexcept : str(_str.data()), next(nullptr){};
        constexpr TreeString(const char* _str, TreeString* _next) noexcept : str(_str), next(_next){};
        constexpr TreeString(const std::string_view& _str, TreeString* _next) noexcept : str(_str.data()), next(_next){};
    };
}

#endif // INERTIA_TREESTRING_HPP
