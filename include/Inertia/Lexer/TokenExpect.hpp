#ifndef INERTIA_TOKENEXPECT_HPP
#define INERTIA_TOKENEXPECT_HPP

#include "Inertia/Lexer/TokenType.hpp"
#include "Inertia/Lexer/TokenStream.hpp"
#include <cstddef>

namespace Inertia{
    enum class expecterr{
        SUCCESS = 0, FAILERR = 1, EOFERR = -1
    };

    template<typename T, size_t n>
    struct expectgroup{
        T group[n];

        template<typename... Args>
        expectgroup(Args... args) noexcept{
            static_assert(sizeof...(args) == n, "Argument count not equals to N");
            size_t i = 0;
            auto unpack_args = [&](auto&&... vals){
                ((group[i++] = vals), ...);
            };
            unpack_args(args...);
        }

        inline expecterr expect(T tt) const noexcept{
            for(size_t i = 0; i < n; i++){
                if(group[i] == tt){
                    return expecterr::SUCCESS;
                }
            }
            return expecterr::FAILERR;
        }
    };

    inline expecterr expect_next(TokenType tt, TokenStream& ss) noexcept{
        if(!ss.peekablef()) return expecterr::EOFERR;
        return ss.peekf().type == tt ? expecterr::SUCCESS : expecterr::FAILERR;
    }

    inline expecterr expect_next(int kwd, TokenStream& ss) noexcept{
        if(!ss.peekablef()) return expecterr::EOFERR;
        int got = (int)ss.peekf().getKeyword();
        if(got != kwd) return expecterr::FAILERR;
        return expecterr::SUCCESS;
    }

    inline expecterr expect(TokenType tt, TokenStream& ss) noexcept{
        TokenType got = ss.current().type;
        if(got == TokenType::TokenEOF) return expecterr::EOFERR;
        return got == tt ? expecterr::SUCCESS : expecterr::FAILERR;
    }

    inline expecterr expect(int kwd, TokenStream& ss) noexcept{
        if(ss.current().type == TokenType::TokenEOF) return expecterr::EOFERR;
        int got = (int)ss.current().getKeyword();
        if(got != kwd) return expecterr::FAILERR;
        return expecterr::SUCCESS;
    }
}

#endif // INERTIA_TOKENEXPECT_HPP
