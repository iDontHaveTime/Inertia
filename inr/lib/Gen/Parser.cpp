// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>
#include <inr/Gen/Driver.h>
#include <inr/Gen/Lexer.h>
#include <inr/Gen/Parser.h>
#include <inr/Gen/Record.h>
#include <inr/Support/Logger.h>

#include <unordered_map>
#include <vector>

namespace inr::gen {

class TokenStream {
    std::vector<token>::const_iterator it_;
    decltype(it_) end_;
    sview fname_;

public:
    TokenStream(const std::vector<token>& tokens, sview fname) noexcept :
        it_(tokens.begin()), end_(tokens.end()), fname_(fname) {}

    template<typename... Args>
    void errorpos(Args&&... args) const {
        log::sendposargs(errs(), log::Level::ERROR, fname_, current().getLine(),
                         current().getColumn(), std::forward<Args>(args)...);
    }

    bool atEnd() const noexcept {
        return it_ == end_;
    }

    const token& current() const noexcept {
        if(atEnd()) return *(it_ - 1);
        return *it_;
    }

    bool advance() {
        if(!atEnd()) {
            ++it_;
            return true;
        }
        return false;
    }

    const token& currentAdvance() noexcept {
        const token& c = current();
        advance();
        return c;
    }

    token::ID getID() const noexcept {
        return current().getID();
    }

    bool expect(token::ID id) {
        return getID() == id;
    }

    bool consume(token::ID id) {
        if(expect(id)) {
            return advance();
        }
        return false;
    }
};

/// Advance forward, if unsuccessful log unexpected.
bool advanceIfNotUnexpected(TokenStream& ts) {
    bool fail = !ts.advance();

    if(fail) {
        ts.errorpos("token stream ended abruptly");
    }

    return fail;
}

const Init* parseIntInit(TokenStream& ts, RecordStorage& result) {
    return result.newInit<IntegerInit>(result, ts.current().getAsInteger());
}

const Init* parseStringInit(TokenStream& ts, RecordStorage& result) {
    return result.newInit<StringInit>(result, ts.current().getAsString());
}

const Init* parseIdentifierInit(TokenStream& ts, RecordStorage& result,
                                std::unordered_map<sview, size_t>& symbolMap,
                                const RecordType* expectedType) {
    sview symbol = ts.current().getAsString();

    auto it = symbolMap.find(symbol);
    if(it != symbolMap.end()) {
        return result.newInit<ArgInit>(it->second);
    }

    if(expectedType->getKind() == RecordType::Kind::Def) {
        const Record* def = result.findDef(symbol);
        if(!def) {
            ts.errorpos("def ", symbol, " was not found");
            return nullptr;
        }
        const RecordDef* rdef = (const RecordDef*)expectedType;
        if(!def->isDerived(rdef->getRecord())) {
            ts.errorpos("def ", symbol, " must be derived from ",
                        rdef->getRecord()->getName());
            return nullptr;
        }
        return result.newInit<DefInit>(result, def);
    }

    return nullptr;
}

const Init* parseEndianInit(TokenStream& ts, RecordStorage& result) {
    token::ID id = ts.getID();

    std::endian endian;
    if(id == token::ID::Little) {
        endian = std::endian::little;
    }
    else if(id == token::ID::Big) {
        endian = std::endian::big;
    }
    else {
        ts.errorpos("endian must be either 'little' or 'big'");
        return nullptr;
    }

    return result.newInit<EndianInit>(result, endian);
}

const Init* parseInit(TokenStream& ts, RecordStorage& result, bool allowIdent,
                      std::unordered_map<sview, size_t>& symbolMap,
                      const RecordType* expectedType);

const Init* parseListInit(TokenStream& ts, RecordStorage& result,
                          std::unordered_map<sview, size_t>& symbolMap,
                          const RecordType* expectedType) {
    if(advanceIfNotUnexpected(ts)) return nullptr;
    if(expectedType->getKind() != RecordType::Kind::List) {
        ts.errorpos("expected type '", expectedType->getAsString(),
                    "' but got 'list<...>'");
        return nullptr;
    }
    ListInit* linit = (ListInit*)result.newInit<ListInit>(expectedType);
    if(ts.expect(token::ID::RightSquare)) {
        return linit;
    }
another_list:
    const Init* candidate =
        parseInit(ts, result, false, symbolMap,
                  ((const RecordList*)expectedType)->getElementTy());
    if(candidate) {
        linit->addInit(candidate);
    }
    else return nullptr;

    if(ts.consume(token::ID::Comma)) goto another_list;

    if(ts.expect(token::ID::RightSquare)) {
        return linit;
    }
    else return nullptr;
}

/// @brief Parses an init from the current token.
/// @param ts Token stream.
/// @param result Required for storage.
/// @param allowIdent Should identifier be allowed (as a string).
/// @param symbolMap Fields in the current class.
/// @param expectedType What type the init should be.
const Init* parseInit(TokenStream& ts, RecordStorage& result, bool allowIdent,
                      std::unordered_map<sview, size_t>& symbolMap,
                      const RecordType* expectedType) {
    const Init* init = nullptr;
    switch(ts.getID()) {
        case token::ID::IntegerLiteral:
            init = parseIntInit(ts, result);
            break;
        case token::ID::Identifier:
            if(allowIdent) init = parseStringInit(ts, result);
            else
                init = parseIdentifierInit(ts, result, symbolMap, expectedType);
            break;
        case token::ID::StringLiteral:
            if(!allowIdent) init = parseStringInit(ts, result);
            break;
        case token::ID::Little:
            [[fallthrough]];
        case token::ID::Big:
            init = parseEndianInit(ts, result);
            break;
        case token::ID::LeftSquare:
            init = parseListInit(ts, result, symbolMap, expectedType);
            break;
        default:
            break;
    }

    if(advanceIfNotUnexpected(ts)) return nullptr;
    return init;
}

const RecordType* parseRecordType(TokenStream& ts, RecordStorage& result);

const RecordType* parseListType(TokenStream& ts, RecordStorage& result) {
    if(!ts.consume(token::ID::LeftArrow)) {
        ts.errorpos("expected '<' after 'list'");
        return nullptr;
    }

    const RecordType* ty = parseRecordType(ts, result);

    if(!ts.consume(token::ID::RightArrow)) {
        ts.errorpos("expected '>' to close 'list'");
        return nullptr;
    }

    return result.getListTy(ty);
}

const RecordType* parseIdentifierType(sview tok, RecordStorage& result) {
    const Record* rec = result.findClass(tok);
    if(!rec) return nullptr;
    return result.getDefTy(rec);
}

const RecordType* parseRecordType(TokenStream& ts, RecordStorage& result) {
    sview tokAsView = ts.current().getAsString();
    switch(ts.currentAdvance().getID()) {
        case token::ID::Endian:
            return result.getEndianTy();
        case token::ID::Int:
            return result.getIntTy();
        case token::ID::String:
            return result.getStringTy();
        case token::ID::List:
            return parseListType(ts, result);
        case token::ID::Identifier:
            return parseIdentifierType(tokAsView, result);
        default:
            return nullptr;
    }
}

RecordField* parseRecordField(TokenStream& ts, RecordStorage& result,
                              Record* record, RecordField::Kind kind,
                              std::unordered_map<sview, size_t>& symbolMap) {
    const RecordType* ty = parseRecordType(ts, result);

    const Init* name = parseInit(ts, result, true, symbolMap, ty);

    if(!name || !name->matches(RecordType::Kind::String)) {
        ts.errorpos("field name must be an identifier in record ",
                    record->getName());
        return nullptr;
    }

    std::pair<RecordField*, size_t> field = record->newField(name, ty, kind);

    if(ts.consume(token::ID::Equals)) {
        const Init* init = parseInit(ts, result, false, symbolMap, ty);
        if(init && !init->matches(ty)) {
            bool err = true;
            sview errType = Init::kindAsString(init->getKind());

            if(init->getKind() == Init::Kind::Arg) {
                const ArgInit* arg = (const ArgInit*)init;
                const RecordField* argField = arg->getArg(record);
                if(!argField) {
                    return field.first;
                }

                if(argField->getType()->getKind() == ty->getKind()) {
                    err = false;
                }
                else {
                    errType = argField->getType()->getAsString();
                }
            }
            if(err) {
                ts.errorpos("expected type '", ty->getAsString(), "' but got '",
                            errType, "' instead in field ",
                            field.first->getName(), " in record ",
                            record->getName());
                init = nullptr;
            }
        }
        else if(!init) {
            ts.errorpos("expected type '", ty->getAsString(),
                        "' but got 'unknown' instead in field ",
                        field.first->getName(), " in record ",
                        record->getName());
        }
        field.first->setValue(init);
    }
    symbolMap[field.first->getName()] = field.second;
    return field.first;
}

bool parseFieldsArgs(TokenStream& ts, RecordStorage& result, Record* record,
                     std::unordered_map<sview, size_t>& symbolMap) {
another_arg:
    if(parseRecordField(ts, result, record, RecordField::Kind::Arg,
                        symbolMap) == nullptr)
        return true;
    if(ts.consume(token::ID::Comma)) goto another_arg;
    return false;
}

bool parseFieldsClass(TokenStream& ts, RecordStorage& result, Record* record,
                      std::unordered_map<sview, size_t>& symbolMap) {
another_field:
    RecordField* rf = parseRecordField(ts, result, record,
                                       RecordField::Kind::Normal, symbolMap);
    if(!rf) return true;

    if(!ts.consume(token::ID::Semicolon)) {
        ts.errorpos("expected ';' in field ", rf->getName(), " in record ",
                    record->getName());
        return true;
    }

    if(!ts.consume(token::ID::RightBrace)) goto another_field;

    return false;
}

bool parseFields(TokenStream& ts, RecordStorage& result, Record* record,
                 RecordField::Kind fieldK,
                 std::unordered_map<sview, size_t>& symbolMap) {
    if(fieldK == RecordField::Kind::Arg && !ts.consume(token::ID::LeftArrow)) {
        ts.errorpos("expected '<' for args in record ", record->getName());
        return true;
    }

    switch(fieldK) {
        case RecordField::Kind::Normal:
            if(parseFieldsClass(ts, result, record, symbolMap)) return true;
            break;
        case RecordField::Kind::Arg:
            if(parseFieldsArgs(ts, result, record, symbolMap)) return true;
            if(!ts.consume(token::ID::RightArrow)) {
                ts.errorpos("expected '>' after args in record ",
                            record->getName());
                return true;
            }
            break;
    }

    return false;
}

const RecordType* nextArgType(std::vector<RecordField>::const_iterator& it,
                              std::vector<RecordField>::const_iterator end) {
    while(it != end) {
        const RecordField& rf = *it++;
        if(rf.getKind() == RecordField::Kind::Arg) {
            return rf.getType();
        }
    }
    return nullptr;
}

bool parseArgsInit(TokenStream& ts, RecordStorage& result,
                   std::unordered_map<sview, size_t>& symbolMap,
                   std::vector<const Init*>& args,
                   std::vector<RecordField>::const_iterator& it,
                   std::vector<RecordField>::const_iterator end) {
another_init:
    const Init* init =
        parseInit(ts, result, false, symbolMap, nextArgType(it, end));
    if(!init) {
        ts.errorpos("couldn't parse args");
        return true;
    }
    args.emplace_back(init);
    if(ts.consume(token::ID::Comma)) goto another_init;
    return false;
}

bool parseInheritanceImpl(TokenStream& ts, RecordStorage& result,
                          Record* record,
                          std::unordered_map<sview, size_t>& symbolMap) {
    const Init* className = parseInit(ts, result, true, symbolMap, nullptr);

    if(!className || !className->matches(RecordType::Kind::String)) {
        ts.errorpos(
            "expected identifier for record name in inheritance for record ",
            record->getName());
        return true;
    }

    sview sclassName = ((const StringInit*)className)->getValue();
    const Record* super = result.findClass(sclassName);
    if(!super) {
        ts.errorpos("record name '", sclassName,
                    "' not found for inheritance in record ",
                    record->getName());
        return true;
    }

    std::vector<const Init*> args;

    auto currentArg = super->getFields().begin();

    if(ts.consume(token::ID::LeftArrow)) {
        parseArgsInit(ts, result, symbolMap, args, currentArg,
                      super->getFields().end());

        if(!ts.consume(token::ID::RightArrow)) {
            ts.errorpos("expected '>' to close args in inheritance of ",
                        super->getName(), " in record ", record->getName());
            return true;
        }
    }

    record->addSuperclass(super, args);

    return false;
}

bool parseInheritance(TokenStream& ts, RecordStorage& result, Record* record,
                      std::unordered_map<sview, size_t>& symbolMap) {
another_inheritance:
    if(parseInheritanceImpl(ts, result, record, symbolMap)) return true;
    if(ts.consume(token::ID::Comma)) goto another_inheritance;
    return false;
}

bool parseRecord(TokenStream& ts, RecordStorage& result, Record::Kind kind) {
    if(advanceIfNotUnexpected(ts)) return true;
    std::unordered_map<sview, size_t> symbolMap;
    const Init* className = parseInit(ts, result, true, symbolMap, nullptr);

    if(!className || !className->matches(RecordType::Kind::String)) {
        ts.errorpos("expected identifier for record name");
        return true;
    }

    Record* record = Record::newRecord(result, className, kind);

    if(ts.expect(token::ID::LeftArrow)) {
        if(kind == Record::Kind::Def) {
            ts.errorpos("args are not allowed in defs, found in ",
                        record->getName());
            return true;
        }
        if(parseFields(ts, result, record, RecordField::Kind::Arg, symbolMap))
            return true;
    }

    if(ts.consume(token::ID::Colon)) {
        if(parseInheritance(ts, result, record, symbolMap)) return true;
    }

    if(ts.consume(token::ID::Semicolon)) return false;

    if(!ts.consume(token::ID::LeftBrace)) {
        ts.errorpos("expected '{' to open record ",
                    ((const StringInit*)className)->getValue());
        return true;
    }

    if(parseFields(ts, result, record, RecordField::Kind::Normal, symbolMap))
        return true;

    return false;
}

bool parseClass(TokenStream& ts, RecordStorage& result) {
    return parseRecord(ts, result, Record::Kind::Class);
}

bool parseDef(TokenStream& ts, RecordStorage& result) {
    return parseRecord(ts, result, Record::Kind::Def);
}

bool parseInclude(GenDriver& driver, TokenStream& ts, RecordStorage& result) {
    if(advanceIfNotUnexpected(ts)) return true;

    if(!ts.expect(token::ID::StringLiteral)) {
        ts.errorpos("expected string literal for include");
        return true;
    }

    sview file = ts.currentAdvance().getAsString();
    if(!ts.consume(token::ID::Semicolon)) {
        ts.errorpos("expected ';' after include");
        return true;
    }

    if(driver.driveFileSpecific(file, result, true) ==
       GenDriver::RequestErr::Internal) {
        ts.errorpos("error driving include");
        return true;
    }

    return false;
}

bool parseStatements(GenDriver& driver, TokenStream& ts,
                     RecordStorage& result) {
    switch(ts.getID()) {
        case token::ID::Class:
            return parseClass(ts, result);
        case token::ID::Def:
            return parseDef(ts, result);
        case token::ID::Include:
            return parseInclude(driver, ts, result);
        default:
            ts.errorpos("unexpected token '", ts.current().getAsString(), '\'');
            ts.advance();
            return true;
    }
}

bool parser::parse(GenDriver& driver, sview fname,
                   const std::vector<token>& toks, RecordStorage& result) {
    TokenStream ts(toks, fname);

    bool err = false;
    while(!ts.atEnd()) {
        if(parseStatements(driver, ts, result)) err = true;
    }

    return err;
}

} // namespace inr::gen

namespace inr {

raw_stream& operator<<(raw_stream& os, std::endian e) {
    constexpr sview endianStrLittle = "little";
    constexpr sview endianStrBig = "big";
    return os << (e == std::endian::little ? endianStrLittle : endianStrBig);
}

} // namespace inr