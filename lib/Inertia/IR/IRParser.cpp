#include "Inertia/IR/IRParser.hpp"
#include "Inertia/Lexer/TokenStream.hpp"

namespace Inertia{

Frame IRParser::parse_tokens(const LexerOutput& tokens){
    TokenStream ss(tokens);
    
    Frame frame;

    return frame;
}

}