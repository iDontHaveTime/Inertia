// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt
#include <inr/Support/Stream.h>
#include <inr/TIR/Printer.h>
#include <inr/TIR/TIRT.h>
#include <isa/Driver/Diag.h>
#include <isa/Lexer/Lexer.h>
#include <isa/Parser/Expression.h>
#include <isa/Parser/Parser.h>

#include <charconv>
#include <memory>
#include <vector>

namespace isa {

bool Parser::parseExpr(std::vector<std::unique_ptr<Value>>& to) {
    if(cur_.id == Token::LParen) {
        consume();
        ExprValue* expr = new ExprValue();
        to.emplace_back(expr);
        while(cur_.id != Token::RParen && cur_.id != Token::EndOfFile) {
            parseAny(expr->getValues());
        }
        if(cur_.id != Token::RParen) {
            std::string msg = "expected ')' to close expression but got '";
            msg += cur_.view;
            msg += '\'';
            newDiag(Diag::Fatal, cur_.view, std::move(msg));
            return false;
        }
        consume();
    }
    else {
        newDiag(Diag::Fatal, cur_.view, "expected '(' to open expression");
        return false;
    }
    return true;
}

bool Parser::parseString(std::vector<std::unique_ptr<Value>>& to) {
    if(cur_.id != Token::String) return false;
    to.emplace_back(new StringValue(cur_.view));
    consume();
    return true;
}

bool Parser::parseTIR(std::vector<std::unique_ptr<Value>>& to) {
    if(cur_.id == Token::Identifier && cur_.view == "tir") {
        consume();

        if(cur_.id != Token::LArrow) {
            newDiag(Diag::Fatal, cur_.view, "expected '<' after 'tir'");
            return false;
        }
        consume();

        if(cur_.id != Token::Identifier) {
            newDiag(Diag::Fatal, cur_.view, "expected a type after 'tir<'");
            return false;
        }
        std::string_view tirType = cur_.view;
        if(tirType.size() > 1 && tirType[0] == 'b') {
            std::string_view bits = tirType.substr(1);
            unsigned bitWidth;
            auto res = std::from_chars(bits.data(), bits.data() + bits.size(),
                                       bitWidth);
            if(res.ec != std::errc()) {
                std::string msg = "couldn't parse tir type '";
                msg += tirType;
                msg += "' due to reason '";
                msg += std::make_error_code(res.ec).message();
                msg += '\'';
                newDiag(Diag::Fatal, tirType, std::move(msg));
                return false;
            }
            to.emplace_back(new TIRValue(inr::TIRT::createBit(bitWidth)));
        }
        else if(tirType == "other") {
            to.emplace_back(new TIRValue(inr::TIRT::createOther()));
        }
        else {
            std::string msg = "unknown tir type '";
            msg += tirType;
            msg += '\'';
            newDiag(Diag::Fatal, tirType, std::move(msg));
            return false;
        }
        consume();

        if(cur_.id != Token::RArrow) {
            newDiag(Diag::Fatal, cur_.view, "expected '>' to close 'tir'");
            return false;
        }
        consume();

        return true;
    }
    return false;
}

bool Parser::parseIdent(std::vector<std::unique_ptr<Value>>& to) {
    if(cur_.id == Token::Identifier) {
        if(cur_.view == "tir") return parseTIR(to);
        to.emplace_back(new IdentValue(cur_.view));
        consume();
        return true;
    }
    return false;
}

bool Parser::parseAny(std::vector<std::unique_ptr<Value>>& to) {
    switch(cur_.id) {
        case Token::Identifier:
            return parseIdent(to);
        case Token::LParen:
            return parseExpr(to);
        case Token::String:
            return parseString(to);
        case Token::LArrow:
            newDiag(Diag::Error, cur_.view, "unexpected '<' encountered");
            consume();
            return false;
        case Token::RArrow:
            newDiag(Diag::Error, cur_.view, "unexpected '>' encountered");
            consume();
            return false;
        case Token::NumberSequence:
            newDiag(Diag::Error, cur_.view,
                    "unexpected number sequence encountered");
            consume();
            return false;
        case Token::UnknownSequence:
            newDiag(Diag::Error, cur_.view,
                    "unknown byte sequence encountered");
            consume();
            return false;
        case Token::UnknownSymbol:
            newDiag(Diag::Error, cur_.view, "unknown symbol encountered");
            consume();
            return false;
        case Token::RParen:
            newDiag(Diag::Error, cur_.view, "unexpected ')' encountered");
            consume();
            return false;
        case Token::EndOfFile:
            return false;
    }
}

std::vector<std::unique_ptr<Value>> Parser::parse() {
    std::vector<std::unique_ptr<Value>> res;
    consume();

    while(cur_.id != Token::EndOfFile) {
        if(!parseExpr(res)) {
            return res;
        }
    }

    return res;
}

static inline void printValue(inr::stream& os, const Value& val) {
    switch(val.getID()) {
        case Value::String:
            os << '"' << ((const StringValue&)val).access() << '"';
            break;
        case Value::Ident:
            os << ((const IdentValue&)val).getIdent();
            break;
        case Value::TIRT: {
            os << "tir<";

            inr::TIRPrinter::printType(os, ((const TIRValue&)val).getType());

            os << '>';
        } break;
        case Value::Expr:
            os << '(';
            for(const std::unique_ptr<Value>& eval :
                ((const ExprValue&)val).getValues()) {
                if(&eval != &((const ExprValue&)val).getValues().front())
                    os << ' ';
                printValue(os, *eval);
            }
            os << ')';
            break;
    }
}

void Parser::printParsed(inr::stream& os,
                         const std::vector<std::unique_ptr<Value>>& exprs) {
    for(const std::unique_ptr<Value>& expr : exprs) {
        printValue(os, *expr);
        os << '\n';
    }
}

} // namespace isa
