#ifndef INERTIA_TYPE_HPP
#define INERTIA_TYPE_HPP

#include "Inertia/Mem/Arenalloc.hpp"
#include <cstdint>
#include <unordered_map>
#include <variant>

namespace Inertia{
    class Type{
    public:
        enum TypeKind : uint8_t {INTEGER, FLOAT, POINTER, VOID} kind;

        TypeKind getKind() const noexcept{
            return kind;
        }

        Type() noexcept = default;
        Type(TypeKind _kind) noexcept : kind(_kind){};

        virtual ~Type() = default;
    };

    class IntegerType : public Type{
    public:
        int width; // bits

        IntegerType(int w) noexcept : Type(INTEGER), width(w){};
    };

    class FloatType : public Type{
    public:
        enum FloatAccuracy {FLOAT_ACC = 1, DOUBLE_ACC = 2} accuracy;

        FloatType(FloatAccuracy _acc) noexcept : Type(FLOAT), accuracy(_acc){};
    };

    class PointerType : public Type{
    public:
        ArenaReference<Type> pointee;

        PointerType(const ArenaReference<Type>& other) noexcept : Type(POINTER), pointee(other){};
    };

    class TypeKey{
    public:
        Type::TypeKind kind;
        std::variant<int, FloatType::FloatAccuracy, ArenaReference<Type>> value;
        TypeKey(Type* t) noexcept : kind(t->getKind()){
            switch(t->getKind()){
                case Type::INTEGER:
                    value = ((IntegerType*)t)->width;
                    break;
                case Type::FLOAT:
                    value = ((FloatType*)t)->accuracy;
                    break;
                case Type::POINTER:
                    value = ((PointerType*)t)->pointee;
                    break;
                default: return;
            }
        }

        TypeKey(Type::TypeKind type) noexcept : kind(type), value(0){};

        bool operator==(const TypeKey& other) const noexcept{
            return kind == other.kind && value == other.value;
        }

        ~TypeKey() noexcept = default;

    };

}

namespace std{
    template<>
    struct hash<Inertia::TypeKey>{
        size_t operator()(const Inertia::TypeKey& key) const noexcept{
            using TK = Inertia::Type;
            size_t h = hash<uint8_t>()(key.kind);
            switch(key.kind){
                case TK::INTEGER:
                    h ^= hash<int>()(get<int>(key.value)) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    break;
                case TK::FLOAT:
                    h ^= hash<int>()((int)(get<Inertia::FloatType::FloatAccuracy>(key.value))) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    break;
                case TK::POINTER:
                    h ^= hash<size_t>()(get<Inertia::ArenaReference<Inertia::Type>>(key.value).get_i()) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    break;
                default:
                    break;
            }
            return h;
        }
    };
}

namespace Inertia{

    class TypeAllocator{
        ArenaAlloc arena;
        std::unordered_map<TypeKey, ArenaPointer<Type>> cache;
    public:
        TypeAllocator(size_t initialSize = 8192) noexcept : arena(initialSize){};

        ArenaPointer<Type> getVoid(void){
            TypeKey key(Type::VOID);

            auto it = cache.find(key);
            if(it != cache.end())
                return it->second;

            auto t = arena.alloc<Type>();
            t->kind = Type::VOID;

            cache[key] = t;
            return t;
        }

        ArenaPointer<IntegerType> getInteger(int width){
            TypeKey key(Type::INTEGER);
            key.value = width;

            auto it = cache.find(key);
            if(it != cache.end())
                return (ArenaPointer<IntegerType>&)it->second;

            auto t = arena.alloc<IntegerType>(width);

            cache[key] = (ArenaPointer<Type>&)t;
            return t;
        }

        ArenaPointer<FloatType> getFloat(FloatType::FloatAccuracy accuracy){
            TypeKey key(Type::FLOAT);
            key.value = accuracy;

            auto it = cache.find(key);
            if(it != cache.end())
                return (ArenaPointer<FloatType>&)it->second;

            auto t = arena.alloc<FloatType>(accuracy);

            cache[key] = (ArenaPointer<Type>&)t;
            return t;
        }

        ArenaPointer<PointerType> getPointer(const ArenaReference<Type>& type){
            TypeKey key(Type::POINTER);
            key.value = type;

            auto it = cache.find(key);
            if(it != cache.end())
                return (ArenaPointer<PointerType>&)it->second;

            auto ptr = arena.alloc<PointerType>(type);

            cache[key] = (ArenaPointer<Type>&)ptr;
            return ptr;
        }

        ArenaAlloc& get_arena() noexcept{
            return arena;
        }

        const ArenaAlloc& get_arena() const noexcept{
            return arena;
        }
    };
}

#endif // INERTIA_TYPE_HPP
