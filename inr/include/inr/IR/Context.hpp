#ifndef INERTIA_CONTEXT_HPP
#define INERTIA_CONTEXT_HPP


/**
 * @file inr/IR/Context.hpp
 * @brief Inertia's context class.
 *
 * This header contains Inertia's IR context class.
 *
 **/

#include "inr/IR/Type.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Map.hpp"

namespace inr{

    /**
     * @brief An essential class for storing context in one class.
     *
     * Keeps one IR context, is thread safe.
     */
    class inrContext{
    public:

        /* Constructors / Operators. */
        inrContext(allocator* _mem = nullptr) noexcept : type_map(_mem){};

        inrContext(const inrContext&) = delete;
        inrContext& operator=(const inrContext&) = delete;

        inrContext(inrContext&&) = default;
        inrContext& operator=(inrContext&&) = default;

        /* Destructor. */
        ~inrContext() noexcept{
            for(auto [_, v] : type_map){
                switch(v->get_kind()){
                    case TypeKind::Integer:
                        type_map.get_allocator()->free((int_type*)v);
                        break;
                    case TypeKind::Float:
                        type_map.get_allocator()->free((float_type*)v);
                        break;
                    case TypeKind::Void:
                        type_map.get_allocator()->free((void_type*)v);
                        break;
                    case TypeKind::Pointer:
                        type_map.get_allocator()->free((ptr_type*)v);
                        break;
                }
            }
        }
        /* End of Constructors | Operators | Destructor. */

        /* Start of fields. */
    private:
        inr::inr_map<size_t, type*> type_map;

        /* End of fields. */
    public:

        type* get_void(){
            type** tp = type_map.get(hash_type_void());
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash_type_void()] = type_map.get_allocator()->alloc<void_type>();
            }
        }

        type* get_integer(size_t width){
            size_t hash = hash_type_int(width);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = type_map.get_allocator()->alloc<int_type>(width);
            }
        }

        type* get_float(float_type::float_variant variant){
            size_t hash = hash_type_float(variant);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = type_map.get_allocator()->alloc<float_type>(variant);
            }
        }

        type* get_pointer(type* pointee){
            size_t hash = hash_type_pointer(pointee);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = type_map.get_allocator()->alloc<ptr_type>(pointee);
            }
        }
    };

}

#endif // INERTIA_CONTEXT_HPP
