#ifndef INERTIA_TYPE_HPP
#define INERTIA_TYPE_HPP

/**
 * @file inr/IR/Type.hpp
 * @brief Inertia's type class.
 *
 * This header contains Inertia's type class.
 *
 **/

#include <cstdint>
#include <cstddef>

namespace inr{

    /**
     * @brief The kind of the type class it is.
     */
    enum class TypeKind : uint8_t{
        Integer,
        Float,
        Void,
        Pointer
    };

    /**
     * @brief Class for determening the type of 
     */
    class type{
        TypeKind kind;    
    public:
        constexpr type(TypeKind _kind) noexcept : kind(_kind){};

        /**
         * @brief Gets the underlying type kind.
         *
         * @return Kind of the type.
         */
        constexpr TypeKind get_kind() const noexcept{
            return kind;
        }
    };

    /**
     * @brief Integer type.
     */
    class int_type : public type{
        size_t width;
    public:
        constexpr int_type(size_t _width) : type(TypeKind::Integer), width(_width){};

        constexpr auto get_width() const noexcept{
            return width;
        }
    };

    /**
     * @brief Float type.
     */
    class float_type : public type{
    public:
        /**
         * @brief Variants of the float type.
         */
        enum class float_variant : uint8_t{
            /* IEEE 754. */
            Float16,
            Float32,
            Float64,
            Float128,
            Float256
        };
    private:
        float_variant variant;
    public:
        constexpr float_type(float_variant _variant) noexcept : type(TypeKind::Float), variant(_variant){};

        constexpr float_variant get_variant() const noexcept{
            return variant;
        }
    };

    /**
     * @brief Void type.
     */
    class void_type : public type{
    public:
        constexpr void_type() noexcept : type(TypeKind::Void){};
    };

    /**
     * @brief Pointer type.
     */
    class ptr_type : public type{
        type* pointee;
    public:
        constexpr ptr_type(type* _pointee) noexcept : type(TypeKind::Pointer), pointee(_pointee){};

        constexpr const type* get_pointee() const noexcept{
            return pointee;
        }
        constexpr type* get_pointee() noexcept{
            return pointee;
        }
    };

    constexpr size_t hash_type_int(const int_type& it) noexcept{
        size_t hsh = (uint8_t)TypeKind::Integer;
        hsh |= it.get_width() << 3;
        return hsh;
    }

    constexpr size_t hash_type_float(const float_type& it) noexcept{
        size_t hsh = (uint8_t)TypeKind::Float;
        hsh |= (size_t)it.get_variant() << 3;
        return hsh;
    }

    constexpr size_t hash_type_void() noexcept{
        return (size_t)((uint8_t)TypeKind::Void);
    }

    inline size_t hash_type_pointer(const ptr_type& pt) noexcept{
        size_t hsh = (uint8_t)TypeKind::Pointer;
        hsh |= (const uintptr_t)pt.get_pointee() << 3;
        return hsh;
    }

    /**
     * @brief Hash the type so it can be accessed easily and allocated once.
     *
     * Hash works like this:
     * first 3 bits are the kind of the type.
     * if void return.
     * if pointee add the address offsetted by 3 bits left.
     * if float add the variant offsetted by 3 bits left.
     * if integer add width offsetted by 3 bits left.
     */
    inline size_t hash_type(type* tp) noexcept{
        switch(tp->get_kind()){
            case TypeKind::Integer:
                return hash_type_int(*((int_type*)tp));
            case TypeKind::Float:
                return hash_type_float(*((float_type*)tp));
            case TypeKind::Void:
                return hash_type_void();
            case TypeKind::Pointer:
                return hash_type_pointer(*((ptr_type*)tp));
            default:
                return 0;
        }
    }
}

#endif // INERTIA_TYPE_HPP
