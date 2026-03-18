// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Parser.h>
#include <inr/Gen/Record.h>
#include <inr/Support/Logger.h>

#include <format>
#include <memory>
#include <unordered_map>
#include <vector>

namespace inr::gen {

constexpr sview PARSER_NAME = "inr-gen-parser";

class TokenStream {
    const std::vector<token>& tokens_;
    size_t current_;

public:
    TokenStream(const std::vector<token>& tokens) :
        tokens_(tokens), current_(0) {}

    bool atEnd() const noexcept {
        return current_ == tokens_.size();
    }

    /// @brief Advances forward in the token vector.
    /// @return True if advanced.
    bool advance() noexcept {
        if(atEnd()) return false;
        ++current_;
        return true;
    }

    /// @brief Advances forward if the expected type is met.
    /// @return True if moved forward.
    bool consume(token::ID id) noexcept {
        if(expect(id)) return advance();
        return false;
    }

    const token& current() const noexcept {
        return tokens_[current_];
    }

    bool expect(token::ID id) const noexcept {
        if(atEnd()) return false;
        return current().getID() == id;
    }
};

bool advanceIfNotError(TokenStream& ts, sview msg) {
    if(!ts.advance()) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME, msg);
        return true;
    }
    return false;
}

bool commaIfNotError(TokenStream& ts, sview msg) {
    if(!ts.consume(token::ID::Comma)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME, msg);
        return true;
    }
    return false;
}

/// @brief Assumes you are after integer token type.
std::optional<unsigned> parseIntegerWidth(TokenStream& ts) {
    if(!ts.consume(token::ID::LeftArrow)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '<' after 'integer'");
        return std::nullopt;
    }
    if(!ts.expect(token::ID::IntegerLiteral)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected an integer literal after '<'");
        return std::nullopt;
    }

    unsigned res = ts.current().getAsInteger();
    ts.advance();

    if(!ts.consume(token::ID::RightArrow)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '>' after an integer literal");
        return std::nullopt;
    }
    return res;
}

using SymbolMap = std::unordered_map<sview, Node*>;

bool parseNewOperand(std::unique_ptr<Node>& root, TokenStream& ts,
                     SymbolMap& symbols) {
    // new Operand(Reg);
    if(!ts.consume(token::ID::LeftParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '(' after 'Operand'");
        return true;
    }

    if(!ts.expect(token::ID::Identifier)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected an identifier");
        return true;
    }

    sview operandName = ts.current().getAsString();

    if(symbols.find(operandName) != symbols.end()) {
        std::string fmt =
            std::format("the symbol {} already exists", operandName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    ts.advance();

    if(!ts.consume(token::ID::RightParen)) {
        std::string fmt =
            std::format("expected ')' after {}", operandName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    if(!ts.consume(token::ID::Semicolon)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ';' after Operand(...)");
        return true;
    }

    symbols[operandName] = root->addNode(new OperandNode(operandName)).get();

    return false;
}

bool parseNewInstructionType(std::unique_ptr<Node>& root, TokenStream& ts,
                             SymbolMap& symbols) {
    // new InstructionType(MOV);
    if(!ts.consume(token::ID::LeftParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '(' after 'InstructionType'");
        return true;
    }

    if(!ts.expect(token::ID::Identifier)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected an identifier");
        return true;
    }

    sview instTName = ts.current().getAsString();

    if(symbols.find(instTName) != symbols.end()) {
        std::string fmt =
            std::format("the symbol {} already exists", instTName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    ts.advance();

    if(!ts.consume(token::ID::RightParen)) {
        std::string fmt =
            std::format("expected ')' after {}", instTName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    if(!ts.consume(token::ID::Semicolon)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ';' after InstructionType(...)");
        return true;
    }

    symbols[instTName] =
        root->addNode(new InstructionTypeNode(instTName)).get();

    return false;
}

bool parseType(std::unique_ptr<Node>& root, TokenStream& ts) {
    if(ts.consume(token::ID::Integer)) {
        std::optional<unsigned> width = parseIntegerWidth(ts);
        if(width.has_value()) {
            root->addNode(new TypeNode(TypeNode::ID::Integer, width.value()));
        }
        else return true;
    }
    else return true;
    return false;
}

bool parseOpDesc(std::unique_ptr<Node>& root, TokenStream& ts,
                 SymbolMap& symbols) {
    // [{Reg, integer<64>}, {Reg, integer<64>}]
    if(!ts.consume(token::ID::LeftSquare)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '[' in operand array");
        return true;
    }

    // {Reg, integer<64>}, {Reg, integer<64>}, ...

    while(!ts.atEnd()) {
        if(!ts.consume(token::ID::LeftBrace)) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "expected '{' to create a new operand description");
            return true;
        }

        if(!ts.expect(token::ID::Identifier)) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "expected an identifier");
            return true;
        }

        sview operandName = ts.current().getAsString();

        auto it = symbols.find(operandName);
        if(it == symbols.end()) {
            std::string fmt =
                std::format("symbol {} not found", operandName.strv());
            log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
            return true;
        }

        if(it->second->getKind() != Node::NodeType::Operand) {
            std::string fmt = std::format("the symbol {} is not an operand",
                                          operandName.strv());
            log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
            return true;
        }

        ts.advance();

        if(commaIfNotError(ts, "expected a comma after operand")) return true;

        if(parseType(
               root->addNode(new OperandDesc((const OperandNode*)it->second)),
               ts))
            return true;

        if(!ts.consume(token::ID::RightBrace)) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "expected '{' to create a new operand description");
            return true;
        }

        if(!ts.consume(token::ID::Comma)) {
            break;
        }
    }

    if(!ts.consume(token::ID::RightSquare)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ')' in Instruction(...)");
        return true;
    }
    return false;
}

bool parseNewInstruction(std::unique_ptr<Node>& root, TokenStream& ts,
                         SymbolMap& symbols) {
    // new Instruction(MOV, [{Reg, integer<64>}, {Reg, integer<64>}]);
    if(!ts.consume(token::ID::LeftParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '(' after 'Instruction'");
        return true;
    }

    if(!ts.expect(token::ID::Identifier)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected an identifier");
        return true;
    }

    sview instTName = ts.current().getAsString();

    auto it = symbols.find(instTName);
    if(it == symbols.end()) {
        std::string fmt = std::format("symbol {} not found", instTName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    if(it->second->getKind() != Node::NodeType::InstructionType) {
        std::string fmt = std::format("the symbol {} is not instruction type",
                                      instTName.strv());
        log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
        return true;
    }

    ts.advance();

    if(commaIfNotError(ts, "expected a comma after instruction type"))
        return true;

    if(parseOpDesc(root->addNode(new InstructionNode(
                       (const InstructionTypeNode*)it->second)),
                   ts, symbols))
        return true;

    if(!ts.consume(token::ID::RightParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ')' in Instruction(...)");
        return true;
    }

    if(!ts.consume(token::ID::Semicolon)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ';' after Instruction(...)");
        return true;
    }

    return false;
}

bool parseNew(std::unique_ptr<Node>& root, TokenStream& ts,
              SymbolMap& symbols) {
    if(ts.atEnd()) return true;

    switch(ts.current().getID()) {
        case token::ID::Operand:
            ts.advance();
            return parseNewOperand(root, ts, symbols);
        case token::ID::InstructionType:
            ts.advance();
            return parseNewInstructionType(root, ts, symbols);
        case token::ID::Instruction:
            ts.advance();
            return parseNewInstruction(root, ts, symbols);
        default:
            return true;
    }
}

bool parseTargetBody(std::unique_ptr<Node>& root, TokenStream& ts) {
    SymbolMap symbols;

    while(!ts.consume(token::ID::RightBrace)) {
        if(!ts.consume(token::ID::New)) {
            std::string fmt = std::format("expected 'new' but got {} instead",
                                          ts.current().getAsString().strv());
            log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
            return true;
        }
        else {
            if(parseNew(root, ts, symbols)) return true;
        }
    }

    return false;
}

bool parseTarget(std::unique_ptr<Node>& root, TokenStream& ts) {
    // For reference it should look like this for target:
    // define target(x86, little, integer<64>)
    if(!ts.consume(token::ID::LeftParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '(' after 'target'");
        return true;
    }

    if(!ts.expect(token::ID::Identifier)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected a target name");
        return true;
    }

    sview targetName = ts.current().getAsString();

    if(advanceIfNotError(ts, "target definition abruptly ended")) return true;
    if(commaIfNotError(ts, "expected a comma after target name")) return true;

    if(!ts.expect(token::ID::Little) && !ts.expect(token::ID::Big)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected endian after target name");
        return true;
    }

    std::endian targetEndian = ts.current().getID() == token::ID::Little
                                   ? std::endian::little
                                   : std::endian::big;

    if(advanceIfNotError(ts, "target definition abruptly ended")) return true;
    if(commaIfNotError(ts, "expected a comma after target endian")) return true;

    unsigned targetPtrWidth = 0;

    if(!ts.consume(token::ID::Integer)) {
        if(!ts.expect(token::ID::IntegerLiteral)) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "expected the integer keyword after target endian for "
                      "pointer width");
            return true;
        }
        else {
            log::send(errs(), log::Level::WARN, PARSER_NAME,
                      "it's preferred to use integer<N> rather than N for "
                      "pointer width");
            targetPtrWidth = ts.current().getAsInteger();
            ts.advance();
        }
    }
    else {
        std::optional<unsigned> opt = parseIntegerWidth(ts);
        if(opt.has_value()) {
            targetPtrWidth = opt.value();
        }
        else return true;
    }

    if(!ts.consume(token::ID::RightParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected ')' in target(...)");
        return true;
    }

    if(!ts.consume(token::ID::LeftBrace)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '{' after target(...)");
        return true;
    }

    return parseTargetBody(
        root->addNode(new TargetNode(targetName, targetEndian, targetPtrWidth)),
        ts);
}

bool parseDefine(std::unique_ptr<Node>& root, TokenStream& ts) {
    if(!ts.consume(token::ID::Target)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "unexpected token after 'define'");
        return true;
    }
    else return parseTarget(root, ts);
}

bool parseExtUnfold(std::vector<token>& ext, TokenStream& ts) {
    if(!ts.consume(token::ID::LeftArrow)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '<' after unfold");
        return true;
    }

    if(!ts.expect(token::ID::Identifier)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected an identifier name in unfold");
        return true;
    }

    sview idenName = ts.current().getAsString();

    if(advanceIfNotError(ts, "unfold definition abruptly ended")) return true;

    if(!ts.consume(token::ID::RightArrow)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '>' after unfold identifier");
        return true;
    }

    unsigned braceC = 1;
    if(!ts.consume(token::ID::LeftBrace)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '{' after unfold<>");
    }

    std::vector<token> toReplace;
    while(true) {
        if(ts.atEnd()) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "no '}' was found to close unfold<>");
            return true;
        }
        else if(ts.consume(token::ID::Unfold)) {
            if(parseExtUnfold(toReplace, ts)) return true;
            continue;
        }
        if(ts.expect(token::ID::LeftBrace)) {
            braceC++;
        }
        else if(ts.expect(token::ID::RightBrace)) {
            braceC--;
            if(!braceC) {
                ts.advance();
                break;
            }
        }
        toReplace.emplace_back(ts.current());
        ts.advance();
    }

    if(!ts.consume(token::ID::LeftParen)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected '(' after unfold expression");
        return true;
    }

    std::vector<arrview<const token>> replacements;
    const token* cur = &ts.current();

    unsigned parenC = 1;
    while(true) {
        if(ts.atEnd()) {
            log::send(errs(), log::Level::ERROR, PARSER_NAME,
                      "no ')' was found to close unfold<>{...}");
            return true;
        }
        if(ts.expect(token::ID::LeftParen)) {
            parenC++;
        }
        else if(ts.expect(token::ID::RightParen)) {
            parenC--;
            if(!parenC) {
                replacements.emplace_back(cur, &ts.current());
                ts.advance();
                break;
            }
        }
        else if(ts.expect(token::ID::Comma)) {
            replacements.emplace_back(cur, &ts.current());

            ts.advance();
            cur = &ts.current();
            continue;
        }
        ts.advance();
    }

    if(!ts.consume(token::ID::Semicolon)) {
        log::send(errs(), log::Level::ERROR, PARSER_NAME,
                  "expected semicolon to terminate unfold");
        return true;
    }

    for(arrview<const token>& replacement : replacements) {
        std::vector<token> replaced;

        for(const token& tok : toReplace) {
            if(tok.getAsString() == idenName) {
                replaced.insert(replaced.end(), replacement.begin(),
                                replacement.end());
            }
            else {
                replaced.emplace_back(tok);
            }
        }

        ext.insert(ext.end(), replaced.begin(), replaced.end());
    }

    return false;
}

std::vector<token> parser::parseExtensions(const std::vector<token>& tokens) {
    std::vector<token> newTokens;

    TokenStream ts(tokens);

    while(!ts.atEnd()) {
        if(ts.consume(token::ID::Unfold)) {
            if(parseExtUnfold(newTokens, ts)) {
                log::send(errs(), log::Level::ERROR, PARSER_NAME,
                          "failed to parse unfold");
                return {};
            }
        }
        else {
            newTokens.emplace_back(ts.current());
            ts.advance();
        }
    }

    return newTokens;
}

std::unique_ptr<Node> parser::parseTokens(const std::vector<token>& toks) {
    std::unique_ptr<Node> root(new Node(Node::NodeType::Root));
    TokenStream ts(toks);

    while(!ts.atEnd()) {
        if(ts.consume(token::ID::Define)) {
            if(parseDefine(root, ts)) {
                log::send(errs(), log::Level::ERROR, PARSER_NAME,
                          "parsing failed");
                break;
            }
        }
        else {
            std::string fmt =
                std::format("expected 'define' but got {} instead",
                            ts.current().getAsString().strv());
            log::send(errs(), log::Level::ERROR, PARSER_NAME, fmt);
            break;
        }
    }

    return root;
}

} // namespace inr::gen

namespace inr {

raw_stream& operator<<(raw_stream& os, std::endian e) {
    return os << (e == std::endian::little ? "little" : "big");
}

} // namespace inr