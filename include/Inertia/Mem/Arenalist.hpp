#ifndef INERTIA_ARENALIST_HPP
#define INERTIA_ARENALIST_HPP

#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    template<typename T>
    struct ArenaNode{
        T val;
        ArenaReference<ArenaNode<T>> next;
    };
    
    template<typename T>
    class ArenaLList{
        ArenaReference<ArenaNode<T>> head;
        ArenaReference<ArenaNode<T>> tail;

        ArenaAlloc* arena = nullptr;

    public:
        ArenaLList() = default;
        ArenaLList(ArenaAlloc* _arena) noexcept: arena(_arena){};

        void set_arena(ArenaAlloc* _arena) noexcept{
            head.unreference();
            tail.unreference();
            arena = _arena;
        }

        void push_back(const T& val){
            if(!arena) return;
            auto node = arena->alloc<ArenaNode<T>>();
            node->val = val;
            node->next = {};
            if(!head){
                head = node;
                tail = node;
            } 
            else{
                tail->next = node;
                tail = node;
            }
        }

        struct iterator{
            ArenaReference<ArenaNode<T>> current;

            iterator& operator++(){ 
                current = current->next; return *this; 
            }
            T& operator*(){ 
                return current->val; 
            }
            bool operator!=(const iterator& other) const{ 
                return current.get() != other.current.get(); 
            }
        };

        iterator begin(){ 
            return {head}; 
        }

        iterator end(){
            return {ArenaReference<ArenaNode<T>>{}};
        }

        ~ArenaLList() = default;
    };
}

#endif // INERTIA_ARENALIST_HPP
