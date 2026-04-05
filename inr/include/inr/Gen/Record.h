// Copyright (c) 2026 Inertia Project
// Distributed under the Boost Software License, Version 1.0.
// See LICENSE file or https://www.boost.org/LICENSE_1_0.txt

#ifndef INERTIA_GEN_RECORD_H
#define INERTIA_GEN_RECORD_H

/// @file Gen/Record.h
/// @brief Contains the nodes for inr-gen.

#include <inr/ADT/ArrView.h>
#include <inr/ADT/StrView.h>
#include <inr/IR/Type.h>

#include <bit>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace inr::gen {

class Record;
class Init;

/// @brief Type of the record (string, int, etc..).
class RecordType {
public:
    /// @brief Enum for possible record types.
    enum class Kind : uint8_t {
        Integer, ///< Any integer, up to 64bit.
        String,  ///< String type, can be allocated or view.
        Endian,  ///< Either little or big endian.
        List,    ///< List type.
        Def,     ///< Points to a def.
        IRType,  ///< Points to an IR type.
        Dag      ///< DAG pattern.
    };

private:
    Kind kind_; ///< Record kind.

public:
    /// @brief Creates a new record type.
    RecordType(Kind kind) noexcept : kind_(kind) {}

    /// @brief Returns the kind of the record.
    Kind getKind() const noexcept {
        return kind_;
    }

    /// @brief Returns the type as string.
    virtual std::string getAsString() const = 0;

    virtual ~RecordType() noexcept = default;
};

/// @brief Represents a DAG record type.
class RecordDag : public RecordType {
public:
    std::string getAsString() const override {
        return "dag";
    }

    RecordDag() noexcept : RecordType(Kind::Dag) {}
};

/// @brief Represents an integer record type.
class RecordDef : public RecordType {
    const Record* def_;

public:
    std::string getAsString() const override;

    const Record* getRecord() const noexcept {
        return def_;
    }

    RecordDef(const Record* def) noexcept : RecordType(Kind::Def), def_(def) {}
};

/// @brief Represents an integer record type.
class RecordInt : public RecordType {
public:
    std::string getAsString() const override {
        return "int";
    }

    RecordInt() noexcept : RecordType(Kind::Integer) {}
};

/// @brief Represents a string record type.
class RecordString : public RecordType {
public:
    std::string getAsString() const override {
        return "string";
    }

    RecordString() noexcept : RecordType(Kind::String) {}
};

/// @brief Represents an endian record type.
class RecordEndian : public RecordType {
public:
    std::string getAsString() const override {
        return "endian";
    }

    RecordEndian() noexcept : RecordType(Kind::Endian) {}
};

/// @brief Represents a list record type.
class RecordList : public RecordType {
    const RecordType* elementTy_;

public:
    std::string getAsString() const override {
        return "list<" + (elementTy_->getAsString() + ">");
    }

    const RecordType* getElementTy() const noexcept {
        return elementTy_;
    }

    RecordList(const RecordType* elementTy) noexcept :
        RecordType(Kind::List), elementTy_(elementTy) {}
};

/// @brief Represents an IR type record type.
class RecordIRType : public RecordType {
public:
    std::string getAsString() const override {
        return "irtype";
    }

    RecordIRType() noexcept : RecordType(Kind::IRType) {}
};

/// @brief Holds the map of classes and a vector of defs.
class RecordStorage {
    /// @brief A map of classes.
    std::unordered_map<sview, std::unique_ptr<Record>> classes_;
    /// @brief A vector of defs.
    std::vector<std::unique_ptr<Record>> defs_;

    std::vector<std::unique_ptr<Init>> initStorage_;

    RecordInt integer_;
    RecordString string_;
    RecordEndian endian_;
    RecordIRType irtype_;
    RecordDag dag_;

    std::vector<std::unique_ptr<RecordType>> typeStorage_;

public:
    arrview<std::unique_ptr<Record>> getDefs() const noexcept {
        return defs_;
    }

    Record* findClass(sview name) {
        auto it = classes_.find(name);
        if(it != classes_.end()) return it->second.get();
        return nullptr;
    }

    const Record* findClass(sview name) const noexcept {
        auto it = classes_.find(name);
        if(it != classes_.end()) return it->second.get();
        return nullptr;
    }

    /// @brief Returns a vector of defs that are derived from the class.
    /// @param record Pointer to the class.
    std::vector<const Record*> getDefsDerivedFrom(const Record* record) const;

    /// @brief Returns a vector of defs that are derived from the class.
    /// @param className Name of the class they must be derived from.
    std::vector<const Record*> getDefsDerivedFrom(sview className) const {
        return getDefsDerivedFrom(findClass(className));
    }

    const Record* findDef(sview name) const noexcept;

    template<typename InitK, typename... Args>
    Init* newInit(Args&&... args) {
        return initStorage_.emplace_back(new InitK(std::forward<Args>(args)...))
            .get();
    }

    const RecordInt* getIntTy() const noexcept {
        return &integer_;
    }

    const RecordDag* getDagTy() const noexcept {
        return &dag_;
    }

    const RecordDef* getDefTy(const Record* def) {
        return (const RecordDef*)typeStorage_
            .emplace_back(std::make_unique<RecordDef>(def))
            .get();
    }

    const RecordString* getStringTy() const noexcept {
        return &string_;
    }

    const RecordEndian* getEndianTy() const noexcept {
        return &endian_;
    }

    const RecordList* getListTy(const RecordType* type) {
        return (const RecordList*)typeStorage_
            .emplace_back(std::make_unique<RecordList>(type))
            .get();
    }

    const RecordIRType* getIRTy() const noexcept {
        return &irtype_;
    }

    friend class Record;
};

/// @brief Base class for all derived inits.
class Init {
public:
    enum class Kind : uint8_t {
        Integer,
        String,
        Endian,
        Arg,
        List,
        Def,
        IRType,
        Dag
    };
    static sview kindAsString(Kind kind) {
        switch(kind) {
            case Kind::Integer:
                return "int";
            case Kind::String:
                return "string";
            case Kind::Endian:
                return "endian";
            case Kind::Arg:
                return "arg";
            case Kind::List:
                return "list";
            case Kind::Def:
                return "record";
            case Kind::IRType:
                return "irtype";
            case Kind::Dag:
                return "dag";
        }
    }

private:
    Kind kind_;

public:
    Init(Kind kind) noexcept : kind_(kind) {}

    Kind getKind() const noexcept {
        return kind_;
    }

    bool matches(RecordType::Kind kind) const noexcept {
        switch(kind) {
            case RecordType::Kind::Integer:
                return kind_ == Kind::Integer;
            case RecordType::Kind::String:
                return kind_ == Kind::String;
            case RecordType::Kind::Endian:
                return kind_ == Kind::Endian;
            case RecordType::Kind::List:
                return kind_ == Kind::List;
            case RecordType::Kind::Def:
                return kind_ == Kind::Def;
            case RecordType::Kind::IRType:
                return kind_ == Kind::IRType;
            case RecordType::Kind::Dag:
                return kind_ == Kind::Dag;
            default:
                return false;
        }
    }

    bool matches(const RecordType* type) const noexcept {
        return matches(type->getKind());
    }

    virtual ~Init() noexcept = default;
};

/// @brief Inits that represent a type.
class TypeInit : public Init {
    const RecordType* type_;

public:
    TypeInit(Kind kind, const RecordType* type) noexcept :
        Init(kind), type_(type) {}

    const RecordType* getType() const noexcept {
        return type_;
    }
};

/// @brief Initializes an irtype field.
///
/// irtype foo = i32;
class IRTypeInit : public TypeInit {
    const Type* type_;

public:
    IRTypeInit(RecordStorage& s, const Type* type) :
        TypeInit(Kind::IRType, s.getIRTy()), type_(type) {}

    const Type* getValue() const noexcept {
        return type_;
    }

    static const Type* get(const Init* init) {
        return ((const IRTypeInit*)init)->getValue();
    }
};

/// @brief Initializes a def field.
///
/// RECORDNAME foo = DEFNAME;
class DefInit : public TypeInit {
    const Record* def_;

public:
    DefInit(RecordStorage& s, const Record* def) :
        TypeInit(Kind::Def, s.getDefTy(def)), def_(def) {}

    const Record* getValue() const noexcept {
        return def_;
    }

    static const Record* get(const Init* init) {
        return ((const DefInit*)init)->getValue();
    }
};

/// @brief Initializes an integer field.
///
/// int foo = 42;
class IntegerInit : public TypeInit {
    int64_t val_;

public:
    IntegerInit(RecordStorage& s, int64_t val) noexcept :
        TypeInit(Kind::Integer, s.getIntTy()), val_(val) {}

    int64_t getValue() const noexcept {
        return val_;
    }

    static int64_t get(const Init* init) {
        return ((const IntegerInit*)init)->getValue();
    }
};

/// @brief Initializes a string field.
///
/// string foo = "bar";
class StringInit : public TypeInit {
    std::variant<sview, std::string> str_;

public:
    StringInit(RecordStorage& s, sview str) noexcept :
        TypeInit(Kind::String, s.getStringTy()), str_(str) {}

    StringInit(RecordStorage& s, std::string str) :
        TypeInit(Kind::String, s.getStringTy()), str_(std::move(str)) {}

    sview getValue() const {
        if(std::holds_alternative<sview>(str_)) {
            return std::get<sview>(str_);
        }
        else {
            const std::string& str = std::get<std::string>(str_);
            return sview(str.data(), str.size());
        }
    }

    static sview get(const Init* init) {
        return ((const StringInit*)init)->getValue();
    }
};

/// @brief Initializes a dag field.
///
/// dag foo = (ADD GR32:$lhs, GR32:$rhs);
class DagInit : public TypeInit {
    const DefInit* operator_;
    std::vector<std::pair<const Init*, const StringInit*>> args_;

public:
    DagInit(RecordStorage& s, const DefInit* op) noexcept :
        TypeInit(Kind::Dag, s.getDagTy()), operator_(op) {}

    void addArg(const Init* lhs, const StringInit* rhs) {
        args_.emplace_back(std::make_pair(lhs, rhs));
    }

    const DefInit* getOperator() const noexcept {
        return operator_;
    }
    const std::vector<std::pair<const Init*, const StringInit*>>& getArgs()
        const noexcept {
        return args_;
    }
};

/// @brief Initializes an endian field.
///
/// endian foo = little;
class EndianInit : public TypeInit {
    std::endian endian_;

public:
    EndianInit(RecordStorage& s, std::endian endian) noexcept :
        TypeInit(Kind::Endian, s.getEndianTy()), endian_(endian) {}

    std::endian getValue() const noexcept {
        return endian_;
    }

    static std::endian get(const Init* init) {
        return ((const EndianInit*)init)->getValue();
    }
};

/// @brief Initializes a list field.
///
/// list<...> foo = [...];
class ListInit : public TypeInit {
    std::vector<const Init*> inits_;

public:
    ListInit(const RecordType* type) : TypeInit(Kind::List, type) {}

    void addInit(const Init* i) {
        inits_.emplace_back(i);
    }

    const std::vector<const Init*>& getInits() const noexcept {
        return inits_;
    }

    static const std::vector<const Init*>& get(const Init* init) {
        return ((const ListInit*)init)->getInits();
    }
};

/// @brief References a template arg, should be resolved.
class ArgInit : public Init {
    size_t ref_;

public:
    ArgInit(size_t ref) noexcept : Init(Kind::Arg), ref_(ref) {}

    const class RecordField* getArg(const Record*) const noexcept;
    size_t getIndex() const noexcept {
        return ref_;
    }
};

/// @brief Any record-related classes that can be identified by their name.
class RecordIdent {
protected:
    const Init* name_;

public:
    RecordIdent(const Init* name) noexcept : name_(name) {}

    sview getName() const {
        if(name_->getKind() != Init::Kind::String)
            throw std::runtime_error(
                "Record identifier name is not a string type");
        return ((const StringInit*)name_)->getValue();
    }
};

/// @brief Record field, could be a normal in-class field or an arg.
class RecordField : public RecordIdent {
public:
    enum class Kind {
        /// @brief Normal class field.
        ///
        /// For example:
        /// class ident {
        ///     string ident = "";
        /// }
        Normal,
        /// @brief Class arg field.
        ///
        /// For example:
        /// class ident<arg, arg, arg...> {...}
        Arg
    };

private:
    const RecordType* type_;
    const Init* value_;
    Kind kind_;

public:
    RecordField(const Init* name, const RecordType* type, Kind kind) :
        RecordIdent(name), type_(type), kind_(kind) {}

    void setValue(const Init* init) {
        value_ = init;
    }

    const RecordType* getType() const noexcept {
        return type_;
    }

    const Init* getValue() const noexcept {
        return value_;
    }

    Kind getKind() const noexcept {
        return kind_;
    }
};

/// @brief Main storage for the fields.
///
/// Records are either classes or defs, each one has their own purposes.
/// Purpose of a class is to represent a storage that can be reused,
/// So classes can be inherited, classes can have args, etc..
/// But defs are final, they inherit a class or classes, then set their args,
/// defs cannot have their own args, they are the final version of that class or
/// classes.
class Record : public RecordIdent {
public:
    enum class Kind : uint8_t { Class, Def };

    friend class ArgInit;

private:
    std::vector<RecordField> fields_;
    std::vector<const Init*> args_;
    std::vector<const Record*> superclasses_;
    Kind kind_;

public:
    Record(const Init* name, Kind kind) : RecordIdent(name), kind_(kind) {}

    std::pair<RecordField*, size_t> newField(const Init* name,
                                             const RecordType* type,
                                             RecordField::Kind kind) {
        if(!name->matches(RecordType::Kind::String)) return {nullptr, 0};

        sview nameStr = ((const StringInit*)name)->getValue();

        size_t idx = 0;
        for(RecordField& field : fields_) {
            if(field.getName() == nameStr) {
                field = RecordField(name, type, kind);
                return {&field, idx};
            }
            idx++;
        }

        idx = fields_.size();
        return {&fields_.emplace_back(name, type, kind), idx};
    }

    std::vector<const Init*>& addSuperclass(const Record* rec) {
        if(!rec || rec == this) return args_;

        superclasses_.emplace_back(rec);
        return args_;
    }

    Kind getKind() const noexcept {
        return kind_;
    }

    const std::vector<RecordField>& getFields() const noexcept {
        return fields_;
    }

    static Record* newRecord(RecordStorage& result, const Init* name,
                             Kind kind) {
        if(name && name->matches(RecordType::Kind::String)) {
            if(kind == Kind::Class) {
                return (result.classes_[((StringInit*)name)->getValue()] =
                            std::make_unique<Record>(name, kind))
                    .get();
            }
            else {
                return result.defs_
                    .emplace_back(std::make_unique<Record>(name, kind))
                    .get();
            }
        }
        return nullptr;
    }

private:
    static const Init* findField(sview name, const Record* record) {
        for(const RecordField& field : record->fields_) {
            if(field.getName() == name &&
               field.getKind() == RecordField::Kind::Normal) {
                return field.getValue();
            }
        }
        return nullptr;
    }

    static size_t getArgCount(const Record* rec) {
        size_t args = 0;
        for(const RecordField& f : rec->fields_) {
            if(f.getKind() == RecordField::Kind::Arg) {
                args++;
            }
        }
        return args;
    }

    const Init* getFieldImpl(sview name) const {
        const Init* field = findField(name, this);

        size_t args = args_.size();
        auto argsEnd = args_.data() + args_.size();

        if(!field) {
            for(auto it = superclasses_.rbegin(); it != superclasses_.rend();
                ++it) {
                const Record* superclass = *it;
                args -= getArgCount(superclass);
                arrview<const Init*> argsSuperclass{args_.data() + args,
                                                    argsEnd};
                const Init* superfield = superclass->getFieldImpl(name);
                if(superfield) {
                    if(superfield->getKind() == Init::Kind::Arg) {
                        superfield = argsSuperclass[((const ArgInit*)superfield)
                                                        ->getIndex()];
                    }
                    return superfield;
                }
            }
        }

        return field;
    }

public:
    const Init* getField(sview name) const {
        if(kind_ != Kind::Def)
            throw std::runtime_error(
                "Fields can only be accessed from defs not classes");
        return getFieldImpl(name);
    }

    bool isDerived(const Record* rec) const noexcept {
        for(const Record* super : superclasses_) {
            if(super == rec) return true;
            if(super->isDerived(rec)) return true;
        }
        return false;
    }

    bool isDerived(sview name) const noexcept {
        for(const Record* rec : superclasses_) {
            if(rec->getName() == name) return true;
            if(rec->isDerived(name)) return true;
        }
        return false;
    }
};

inline std::vector<const Record*> RecordStorage::getDefsDerivedFrom(
    const Record* record) const {
    std::vector<const Record*> recs;

    for(const std::unique_ptr<Record>& def : defs_) {
        if(def->isDerived(record)) recs.push_back(def.get());
    }

    return recs;
}

inline const Record* RecordStorage::findDef(sview name) const noexcept {
    for(const std::unique_ptr<Record>& rec : defs_) {
        if(rec->getName() == name) return rec.get();
    }
    return nullptr;
}

inline const RecordField* ArgInit::getArg(const Record* rec) const noexcept {
    return &rec->fields_[ref_];
}

inline std::string RecordDef::getAsString() const {
    return def_->getName().str();
}

} // namespace inr::gen

namespace inr {
raw_stream& operator<<(raw_stream&, std::endian);
}

#endif // INERTIA_GEN_RECORD_H
