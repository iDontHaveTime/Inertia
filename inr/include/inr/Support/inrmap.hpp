#ifndef INERTIA_INRMAP_HPP
#define INERTIA_INRMAP_HPP

/**
 * @file inr/Support/inrmap.hpp
 * @brief Inertia's hash map implementation.
 *
 * A simple hash map using chaining for collision resolution.
 **/

#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrvector.hpp"

#include <concepts>
#include <cstddef>

namespace inr{

    /**
     * @brief Generic hash function. Specialize for custom types.
     */
    template<typename T>
    struct hash{
        size_t operator()(const T& value) const noexcept;
    };

    template<typename T>
    struct hash<T*>{
        size_t operator()(T* ptr) const noexcept{
            return (size_t)(uintptr_t)ptr;
        }
    };

    template<std::integral T>
    struct hash<T>{
        size_t operator()(T value) const noexcept{
            return (size_t)value;
        }
    };
    
    template<typename K, typename V>
    class inr_map{
        struct entry{
            K key;
            V value;
            size_t hash;
            entry* next;

            entry(const K& k, const V& v, size_t h) : key(k), value(v), hash(h), next(nullptr){};
  
        };

    public:
        inr_map(const inr_map&) = default;
        inr_map& operator=(const inr_map&) = default;

        inr_map(inr_map&& other) noexcept : buckets(std::move(other.buckets)), count(other.count), hashf(std::move(other.hashf)){
            other.count = 0;
        }

        inr_map& operator=(inr_map&& other) noexcept{
            if(this != &other){
                clear();
                buckets = std::move(other.buckets);
                hashf = std::move(other.hashf);

                count = other.count;
                other.count = 0;
            }
            return *this;
        }
        
    private:
        using hasher = inr::hash<K>;

        inr_vec<entry*> buckets;
        size_t count;
        hasher hashf;

        size_t get_bucket_index(size_t hash) const noexcept{
            return hash % buckets.size();
        }

        size_t hash_key(const K& key) const noexcept{
            return hashf(key);
        }

        void maybe_resize(){
            if(count > (buckets.size() * 3) >> 2){
                resize(buckets.size() << 1);
            }
        }

        void resize(size_t new_size){
            inr_vec<entry*> new_buckets(buckets.get_allocator());

            new_buckets.reserve(new_size);
            for(size_t i = 0; i < new_size; i++){
                new_buckets.push_back(nullptr);
            }

            for(size_t i = 0; i < buckets.size(); i++){
                entry* e = buckets[i];

                while(e){
                    entry* next = e->next;

                    size_t new_idx = e->hash % new_size;
                    e->next = new_buckets[new_idx];
                    new_buckets[new_idx] = e;
                
                    e = next;
                }
            }

            buckets = std::move(new_buckets);
        }
    public:
        /**
         * @brief Creates a new hash map.
         * 
         * @param _alloc Allocator to use.
         * @param initial_buckets Initial number of buckets (default 16).
         */
        inr_map(allocator* _alloc = nullptr, size_t initial_buckets = 16) : buckets(_alloc), count(0){
            buckets.reserve(initial_buckets);
            for(size_t i = 0; i < initial_buckets; i++){
                buckets.push_back(nullptr);
            }
        }

        /**
         * @brief Inserts a key-value pair. Overwrites if key exists.
         */
        void insert(const K& key, const V& value){
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(hash);

            for(entry* e = buckets[idx]; e; e = e->next){
                if(e->hash == hash && e->key == key){
                    e->value = value;
                    return;
                }
            }

            entry* new_entry = (*buckets.get_allocator()).template alloc<entry>(key, value, hash);
            new_entry->next = buckets[idx];
            buckets[idx] = new_entry;
            count++;

            maybe_resize();
        }

        /**
         * @brief Gets value by key. Returns nullptr if not found.
         *
         * @return Pointer if found, nullptr if not.
         */
        V* get(const K& key) noexcept{
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(hash);
            
            for(entry* e = buckets[idx]; e; e = e->next){
                if(e->hash == hash && e->key == key){
                    return &e->value;
                }
            }
            return nullptr;
        }

        /**
         * @brief Gets value by key. Returns nullptr if not found. Const version.
         *
         * @return Pointer if found, nullptr if not.
         */
        const V* get(const K& key) const noexcept{
            size_t hash = hash_key(key);
            size_t idx = get_bucket_index(hash);
            
            for(entry* e = buckets[idx]; e; e = e->next){
                if(e->hash == hash && e->key == key){
                    return &e->value;
                }
            }
            return nullptr;
        }

        /**
         * @brief Checks if key exists.
         */
        bool contains(const K& key) const noexcept{
            return this->get(key) != nullptr;
        }

        /**
         * @brief Gets the number of items in the map.
         */
        size_t size() const noexcept{
            return count;
        }

        /**
         * @brief Operator[] for convenience.
         */
        V& operator[](const K& key){
            V* val = get(key);
            if(val){
                return *val;
            }

            V default_val = V();
            insert(key, default_val);
            return *get(key);
        }

        void clear() noexcept{
            allocator* alloc = buckets.get_allocator();

            for(size_t i = 0; i < buckets.size(); i++){
                entry* e = buckets[i];

                while(e){
                    entry* next = e->next;
                    alloc->free(e);
                    e = next;
                }
            }
        }

        hasher hash_function() const noexcept{
            return hashf;
        }

        size_t bucket_count() const noexcept{
            return count;
        }

        /**
         * @brief Gets the internal allocator.
         */
        allocator* get_allocator() const noexcept{
            return buckets.get_allocator();
        }

        ~inr_map() noexcept{
            clear();
        }
    };

}

#endif // INERTIA_INRMAP_HPP
