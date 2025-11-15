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
    template<inertia_allocator _ctx_alloc_ = allocator>
    class inrContext : private _ctx_alloc_{
    public:

        /* Constructors / Operators. */
        inrContext() noexcept = default;

        inrContext(const inrContext&) = delete;
        inrContext& operator=(const inrContext&) = delete;

        inrContext(inrContext&&) = default;
        inrContext& operator=(inrContext&&) = default;

        /* Destructor. */
        ~inrContext() noexcept{
            for(auto [_, v] : type_map){
                switch(v->get_kind()){
                    case TypeKind::Integer:
                        _ctx_alloc_::free((int_type*)v);
                        break;
                    case TypeKind::Float:
                        _ctx_alloc_::free((float_type*)v);
                        break;
                    case TypeKind::Void:
                        _ctx_alloc_::free((void_type*)v);
                        break;
                    case TypeKind::Pointer:
                        _ctx_alloc_::free((ptr_type*)v);
                        break;
                }
            }
        }
        /* End of Constructors | Operators | Destructor. */

        /* Start of fields. */
    private:
        inr::inr_map<size_t, type*, _ctx_alloc_> type_map;

        /* End of fields. */
    public:

        type* get_void(){
            type** tp = type_map.get(hash_type_void());
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash_type_void()] = _ctx_alloc_::template alloc<void_type>();
            }
        }

        type* get_integer(size_t width){
            size_t hash = hash_type_int(width);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = _ctx_alloc_::template alloc<int_type>(width);
            }
        }

        type* get_float(float_type::float_variant variant){
            size_t hash = hash_type_float(variant);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = _ctx_alloc_::template alloc<float_type>(variant);
            }
        }

        type* get_pointer(type* pointee){
            size_t hash = hash_type_pointer(pointee);
            type** tp = type_map.get(hash);
            if(tp){
                return *tp;
            }
            else{
                return type_map[hash] = _ctx_alloc_::template alloc<ptr_type>(pointee);
            }
        }
    };

}

#endif // INERTIA_CONTEXT_HPP
