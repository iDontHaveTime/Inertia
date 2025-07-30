#ifndef INERTIA_TYPE_HPP
#define INERTIA_TYPE_HPP

#include "Inertia/Mem/Arenalloc.hpp"
#include <cstdint>
#include <unordered_map>

namespace Inertia{
    class Type{
    public:
        enum TypeKind : uint8_t {INTEGER, FLOAT, POINTER, VOID} kind;

        TypeKind getKind() const noexcept{
            return kind;
        }

        Type() = default;
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
        enum FloatAccuracy {FLOAT_ACC, DOUBLE_ACC} accuracy;

        FloatType(FloatAccuracy _acc) noexcept : Type(FLOAT), accuracy(_acc){};
    };  

    class PointerType : public Type{
    public:
        Type* pointee;

        PointerType(Type* other) noexcept : Type(POINTER), pointee(other){}; 
    };

    class TypeKey{
    public:
        Type::TypeKind kind;
        union{
            int int_width;
            FloatType::FloatAccuracy f_type;
            Type* pointee;
        };
        TypeKey(Type* t) noexcept : kind(t->getKind()){
            switch(t->getKind()){
                case Type::INTEGER:
                    int_width = ((IntegerType*)t)->width;
                    break;
                case Type::FLOAT:
                    f_type = ((FloatType*)t)->accuracy;
                    break;
                case Type::POINTER:
                    pointee = ((PointerType*)t)->pointee;
                    break;
                default: return;
            }
        }

        TypeKey(Type::TypeKind type) noexcept : kind(type){};

        bool operator==(const TypeKey& other) const noexcept{
            if(kind != other.kind) return false;
            switch(kind){
                case Type::INTEGER: return int_width == other.int_width;
                case Type::FLOAT:   return f_type == other.f_type;
                case Type::POINTER: return pointee == other.pointee;
                default: return true;
            }
        }
        
    };

}

namespace std{
    template<>
    struct hash<Inertia::TypeKey>{
        size_t operator()(const Inertia::TypeKey& key) const noexcept{
            using TK = Inertia::Type;
            size_t h = std::hash<uint8_t>()(key.kind);
            switch(key.kind){
                case TK::INTEGER:
                    h ^= std::hash<int>()(key.int_width) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    break;
                case TK::FLOAT:
                    h ^= std::hash<int>()(key.f_type) + 0x9e3779b9 + (h << 6) + (h >> 2);
                    break;
                case TK::POINTER:
                    h ^= std::hash<Inertia::Type*>()(key.pointee) + 0x9e3779b9 + (h << 6) + (h >> 2);
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
        std::unordered_map<TypeKey, Type*> cache;
    public:
        TypeAllocator(size_t initialSize = 1024) : arena(initialSize){};

        IntegerType* getInteger(int width){
            TypeKey key(Type::INTEGER);
            key.int_width = width;

            auto it = cache.find(key);
            if(it != cache.end())
                return (IntegerType*)(it->second);

            IntegerType* type;
            arena.alloc(sizeof(IntegerType), type, width);

            cache[key] = type;
            return type;
        }

        PointerType* getPointer(Type* type){
            TypeKey key(Type::POINTER);
            key.pointee = type;

            auto it = cache.find(key);
            if(it != cache.end())
                return (PointerType*)(it->second);

            PointerType* ptr;
            arena.alloc(sizeof(PointerType), ptr, type);

            cache[key] = ptr;
            return ptr;
        }
    };
}

#endif // INERTIA_TYPE_HPP
