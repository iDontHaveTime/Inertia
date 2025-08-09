#ifndef INERTIA_ARENALIST_HPP
#define INERTIA_ARENALIST_HPP

#include "Inertia/Mem/Arenalloc.hpp"

namespace Inertia{
    template<typename T>
    struct ArenaNode{
        ArenaReference<T> val;
        ArenaReference<ArenaNode<T>> next;
    };
    
    template<typename T>
    class ArenaLList{
        ArenaReference<ArenaNode<T>> head;
        ArenaReference<ArenaNode<T>> tail;
        ArenaAlloc* allocator = nullptr;

    public:
        ArenaLList() = default;
        ArenaLList(ArenaAlloc* alloc) noexcept : allocator(alloc){};

        void set_arena(ArenaAlloc* alloc) noexcept{
            head.unreference();
            tail.unreference();
            allocator = alloc;
        }

        void push_back(ArenaReference<T> ref){
            if(!allocator) return;
            ArenaReference<ArenaNode<T>> node = allocator->alloc<ArenaNode<T>>();
            node->val = ref;
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

        template<typename Y>
        void push_back_as(const Y& value){
            if(!allocator) return;
            ArenaReference<Y> item = allocator->alloc<Y>(value);
            ArenaReference<ArenaNode<T>> node = allocator->alloc<ArenaNode<T>>();
            node->val = item.__unsafe_cast__();
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

        void push_back(const T& value){
            if(!allocator) return;
            auto item = allocator->alloc<T>(value);
            ArenaReference<ArenaNode<T>> node = allocator->alloc<ArenaNode<T>>();
            node->val = item;
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

        struct const_iterator{
            ArenaReference<ArenaNode<T>> current;

            const_iterator& operator++() noexcept{ 
                current = current->next; 
                return *this; 
            }

            const ArenaReference<T>& operator*() const noexcept{
                return current->val;
            }

            bool operator!=(const const_iterator& other) const noexcept{ 
                return current.get() != other.current.get(); 
            }

            const ArenaReference<T>& operator->() const noexcept{
                return current->val;
            }
        };

        struct iterator{
            ArenaReference<ArenaNode<T>> current;

            iterator& operator++() noexcept{ 
                current = current->next; 
                return *this; 
            }

            ArenaReference<T>& operator*() noexcept{
                return current->val;
            }

            bool operator!=(const iterator& other) const noexcept{ 
                return current.get() != other.current.get(); 
            }

            ArenaReference<T>& operator->() noexcept{
                return current->val;
            }
        };

        const ArenaReference<ArenaNode<T>>& get_head() const noexcept{
            return head;
        }

        const ArenaReference<ArenaNode<T>> get_tail() const noexcept{
            return tail;
        }

        iterator begin() noexcept{ 
            return {head}; 
        }

        iterator end() noexcept{
            return {ArenaReference<ArenaNode<T>>{}};
        }

        const_iterator begin() const noexcept{ 
            return {head}; 
        }

        const_iterator end() const noexcept{
            return {ArenaReference<ArenaNode<T>>{}};
        }

        ~ArenaLList() = default;
    };
}

#endif // INERTIA_ARENALIST_HPP
