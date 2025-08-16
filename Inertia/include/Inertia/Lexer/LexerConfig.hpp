#ifndef INERTIA_LEXERCONFIG_HPP
#define INERTIA_LEXERCONFIG_HPP

#include "Inertia/Lexer/Lexer.hpp"
#include "Inertia/Target/TargetKeywords.hpp"

namespace Inertia{
    enum class LexerPreConfigs{
        IR, TARGET
    };
    /* This class preconfigs the lexer */
    class LexerConfig{
        void configure_target(Lexer& lexr) noexcept{
            lexr.line_comment = TokenType::SlashSlash;
            lexr.multiline_end = TokenType::StarSlash;
            lexr.multiline_start = TokenType::SlashStar;
            lexr.throw_eof = true;

            lexr.SetKeywords(CreateTargetKeywordMap());
        }
        void configure_ir(Lexer& lexr) noexcept{
            lexr.line_comment = TokenType::SlashSlash;
            lexr.multiline_end = TokenType::StarSlash;
            lexr.multiline_start = TokenType::SlashStar;
            lexr.throw_eof = true;
        }
    public:
        LexerConfig() noexcept = default;

        LexerConfig(Lexer& lexr, LexerPreConfigs pc) noexcept{
            configure(lexr, pc);
        }

        void configure(Lexer& lexr, LexerPreConfigs pc) noexcept{
            switch(pc){
                case LexerPreConfigs::IR:
                    configure_ir(lexr);
                    break;
                case LexerPreConfigs::TARGET:
                    configure_target(lexr);
                    break;
            }
        }
    };
}

#endif // INERTIA_LEXERCONFIG_HPP
