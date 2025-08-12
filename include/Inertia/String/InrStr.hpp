#ifndef INERTIA_INRSTR_HPP
#define INERTIA_INRSTR_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <ostream>

namespace Inertia{
    // CUTS OFF IF REACHES MAX
    template<uint32_t max = 16>
    class inrstr{
        char buff[max+1];
        size_t pos = 0;
    public:
        
        inrstr() noexcept{
            buff[0] = '\0';
        }

        inrstr(const char* str) noexcept{
            if(!str) return;
            size_t slen = strlen(str);
            if(slen == 0){
                buff[0] = '\0';
                return;
            }
            for(size_t i = 0; i < slen; i++){
                buff[pos++] = str[i];
                buff[pos] = '\0';
                if(pos == max){
                    return;
                }
            }
        }

        inrstr(inrstr& rhs) noexcept = default;
        inrstr& operator=(inrstr& rhs) noexcept = default;

        inrstr(inrstr&& rhs) noexcept{
            if(this != &rhs){
                memcpy(buff, rhs.buff, rhs.pos);
                pos = rhs.pos;
                rhs.pos = 0;
            }
            return *this;
        }

        inrstr& operator=(inrstr&& rhs) noexcept{
            if(this != &rhs){
                memcpy(buff, rhs.buff, rhs.pos);
                pos = rhs.pos;
                rhs.pos = 0;
            }
            return *this;
        }

        inline size_t length() const noexcept{
            return pos;
        }

        inline size_t size() const noexcept{
            return pos;
        } 

        inline bool eof() const noexcept{
            return pos == max;
        }

        void operator+=(char c) noexcept{
            if(pos < max){
                buff[pos++] = c;
                buff[pos] = '\0';
            }
        }

        operator const char*() const noexcept{
            return buff;
        }

        char operator[](size_t index) noexcept{
            if(index >= max){
                return '\0';
            }
            return buff[index];
        }

        friend std::ostream& operator<<(std::ostream& lhs, const inrstr& rhs){
            if(rhs.pos == 0) return lhs;
            for(size_t i = 0; i < rhs.pos; i++){
                lhs<<rhs.buff[i];
            }
        }

        ~inrstr() noexcept = default;
    };
}

#endif // INERTIA_INRSTR_HPP
