#ifndef TOKENTYPE_HPP
#define TOKENTYPE_HPP

#include <cstdint>

namespace Inertia{
    enum class TokenType : uint32_t{
        Special, // base
        Alpha, // base
        Number, // base
        UTF_2,
        UTF_3,
        UTF_4,
        UTF_Continue,

        Exclamation = '!', // !
        At = '@', // @
        Hashtag = '#', // #
        Dollar = '$', // $
        Percent = '%', // %
        Caret = '^', // ^
        And = '&', // &
        Star = '*', // *
        LeftParen = '(', // (
        RightParen = ')', // )
        Minus = '-', // -
        Underscore = '_', // _ usually alpha though
        Equals = '=', // =
        Plus = '+', // +
        Or = '|', // |
        Backslash = '\\', // \ (needs escaping)
        Tag = '`', // `
        LeftSquare = '[', // [
        RightSquare = ']', // ]
        LeftBrace = '{', // {
        RightBrace = '}', // }
        Quote = '\'', // ' (needs escaping)
        DoubleQuote = '"', // "
        Semicolon = ';', // ;
        Colon = ':', // :
        Question = '?', // ?
        Slash = '/', // /
        Left = '<', // <
        Right = '>', // >
        Dot = '.', // .
        Comma = ',', // ,
        Tilde = '~', // ~

        LogicalAnd, // &&
        LogicalOr, // ||

        PlusPlus, // ++
        MinusMinus, // --
        StarEquals, // *=
        SlashEquals, // /=
        PercentEquals, // %=
        PlusEquals, // +=
        MinusEquals, // -=
        AmpersandEquals, // &=
        PipeEquals, // |=
        CaretEquals, // ^=
        LeftShiftEquals, // <<=
        RightShiftEquals, // >>=
        EqualsEquals,
        GreaterEquals,
        LessEquals,

        StarSlash, // */
        SlashStar, // /*
        SlashSlash, // //

        Arrow, // ->
        DoubleColon, // ::
        Ellipsis, // ...
        LeftShift, // <<
        RightShift, // >>

        StringLiteral, // str
        CharLiteral, // char
        IntegerLiteral, // int
        FloatLiteral, // float
        HexLiteral, // hex
        BinaryLiteral, // binary

        Keyword,

        #if defined(NEW_TOKEN_TYPE)
        NEW_TOKEN_TYPE
        #endif
    };
}

#endif // TOKENTYPE_HPP
