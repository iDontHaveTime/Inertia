#ifndef TOKENBUILD_HPP
#define TOKENBUILD_HPP

#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <string>
#include <ostream>

namespace Inertia{
    struct TokenBuild{
        char buffer[1024] = {0};
        size_t index = 0;

        TokenBuild() : index(0){};

        inline void add_char(char c) noexcept{
            if(index < sizeof(buffer))
                buffer[index++] = c;
        }

        inline void clear() noexcept{
            index = 0;
        }

        inline const char* c_str() noexcept{
            if(index < sizeof(buffer)){
                *(buffer + index) = '\0';
            }
            else return nullptr;
            return buffer;
        }

        bool operator==(const char* rhs) const noexcept{
            if(!rhs) return false;
            size_t rhl = strlen(rhs);
            if(rhl != index) return false;
            return memcmp(buffer, rhs, index) == 0;
        }

        bool operator==(const std::string& rhs) const noexcept{
            if(rhs.empty()) return false;
            if(index != rhs.length()) return false;
            return memcmp(rhs.data(), buffer, index) == 0;
        }

        bool operator==(const std::string_view& rhs) const noexcept{
            if(rhs.empty()) return false;
            if(index != rhs.length()) return false;
            return memcmp(buffer, rhs.data(), index) == 0;
        }

        operator std::string_view() const{
            return {buffer, index};
        }

        inline const char* data() const noexcept{
            return buffer;
        }
        inline size_t length() const noexcept{
            return index;
        }

        inline bool matching(char lhs, char rhs) const noexcept{
            if(index != 2) return false;
            return (lhs == buffer[0] && rhs == buffer[1]);
        }

        inline bool matching(char lhs, char mid, char rhs) const noexcept{
            if(index != 3) return false;
            return (lhs == buffer[0] && mid == buffer[1] && rhs == buffer[2]);
        }

        friend std::ostream& operator<<(std::ostream& lhs, const TokenBuild& rhs){
            if(rhs.index == 0) return lhs;
            for(size_t i = 0; i < rhs.index; i++){
                lhs<<rhs.buffer[i];
            }
            return lhs;
        }
    };
}

#endif // TOKENBUILD_HPP
