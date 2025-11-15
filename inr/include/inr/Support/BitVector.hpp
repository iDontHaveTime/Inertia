#ifndef INERTIA_BITVECTOR_HPP
#define INERTIA_BITVECTOR_HPP

/**
 * @file inr/Support/BitVector.hpp
 * @brief Inertia's bit vector class.
 *
 * This header contains Inertia's vector class.
 * This class is like a vector class but for bits instead of bytes.
 *
 **/

#include "inr/Defines/CommonTypes.hpp"
#include "inr/Support/Alloc.hpp"
#include "inr/Support/Cexpr.hpp"
#include "inr/Support/Stream.hpp"
#include "inr/Support/Vector.hpp"

#include <cstdint>

namespace inr{

    /**
     * @brief Class for referencing a bit.
     */
    template<std::unsigned_integral T>
    class bitref{
        T* ref;
        uint32_t bit;

        constexpr bool pre_test() const noexcept{
            if(ref == nullptr) return true;
            return false;
        }
    public:

        constexpr bitref() noexcept : ref(nullptr), bit(0){};
        constexpr bitref(T* src, uint32_t i) noexcept : ref(src), bit(i){};

        constexpr bitref(const bitref&) noexcept = default;
        constexpr bitref& operator=(const bitref&) noexcept = default;

        constexpr bitref(bitref&& other) noexcept{
            ref = other.ref;
            bit = other.bit;

            other.ref = nullptr;
        }

        constexpr bitref& operator=(bitref&& other) noexcept{
            if(this != &other){
                ref = other.ref;
                bit = other.bit;

                other.ref = nullptr;
            }
            return *this;
        }

        constexpr bool test() const noexcept{
            if(pre_test()) return false;
            return (*ref & ((T)1 << bit)) > 0;
        }

        constexpr bool get() const noexcept{
            return test();
        }

        constexpr void enable() noexcept{
            if(pre_test()) return;
            *ref |= (T)1 << bit;
        }

        constexpr void disable() noexcept{
            if(pre_test()) return;
            *ref &= ~((T)1 << bit);
        }

        constexpr void set(bool v) noexcept{
            if(v){
                enable();
            }
            else{
                disable();
            }
        }

        constexpr bitref& operator=(bool v) noexcept{
            set(v);
            return *this;
        }

        constexpr ~bitref() noexcept = default;
    };

    /**
     * @brief Resizable container for bits, unlike bitset this is runtime.
     */
    template<inertia_allocator _bit_alloc_ = allocator>
    class bit_vector : private _bit_alloc_{
        using limb = uintptr_t;

        inline_vec<limb, 1, _bit_alloc_> base;
        size_t bit;
        using word = decltype(base)::word;
    public:

        bit_vector() noexcept : base(), bit(0){};

        bit_vector(const bit_vector&) = default;
        bit_vector& operator=(const bit_vector&) = default;

        bit_vector(bit_vector&& other) noexcept{
            base = std::move(other.base);
            bit = other.bit;
            other.bit = 0;
        }

        bit_vector& operator=(bit_vector&& other) noexcept{
            if(this != &other){
                base = std::move(other.base);
                bit = other.bit;
                other.bit = 0;
            }
            return *this;
        }

        /**
         * @brief Reserves the amount of bits specified.
         */
        void reserve(word goal){
            if(goal > bit){
                base.reserve(
                    (calculate_bytes_up(goal) + (sizeof(limb)-1)) >> bit_to_shift<limb>(sizeof(limb))
                );
            }
        }

        /**
         * @brief Returns the amount of bits in the vector that are active.
         * @return Amount of bits.
         */
        word size() const noexcept{
            return bit;
        }

        /**
         * @brief The amount of bits currently allocated in the vector.
         * @return Amount of bits allocated.
         */
        word capacity() const noexcept{
            return base.capacity() * widthof<limb>();
        }

        bitref<limb> operator[](array_access n) noexcept{
            if(n >= bit) return {};

            return bitref<limb>(base.data() + (n >> bit_to_shift<limb>(widthof<limb>())), n & (widthof<limb>()-1));
        }

        const bitref<const limb> operator[](array_access n) const noexcept{
            if(n >= bit) return {};

            return bitref<const limb>(base.data() + (n >> bit_to_shift<limb>(widthof<limb>())), n & (widthof<limb>()-1));
        }

        /**
         * @brief Alternative to operator[].
         *
         * @param n Index to access the bit at.
         */
        bitref<limb> at(array_access n) noexcept{
            return (*this)[n];
        }

        /**
         * @brief Alternative to operator[] const version.
         *
         * @param n Index to access the bit at.
         */
        const bitref<const limb> at(array_access n) const noexcept{
            return (*this)[n];
        }

        /**
         * @brief Returns a const reference for the first bit.
         */
        const bitref<const limb> front() const noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a read/write reference for the first bit.
         */
        bitref<limb> front() noexcept{
            return (*this)[0];
        }

        /**
         * @brief Returns a const reference for the last bit.
         */
        const bitref<const limb> back() const noexcept{
            return (*this)[bit-1];
        }

        /**
         * @brief Returns a read/write reference for the last bit.
         */
        bitref<limb> back() noexcept{
            return (*this)[bit-1];
        }

        /**
         * @brief Removes the last bit.
         */
        void pop_back() noexcept{
            if(bit) bit--;
        }

        /**
         * @brief Clears all bits.
         */
        void clear(bool free_everything = false) noexcept{
            bit = 0;
            base.clear(free_everything);
        }

        /**
         * @brief Gets the internal allocator.
         */
        _bit_alloc_ get_allocator() const noexcept{
            return _bit_alloc_{};
        }

        void emplace_back(bool v){
            if(size() >= capacity()){
                base.emplace_back(0);
            }

            bitref<limb> b(base.data() + (bit >> bit_to_shift<limb>(widthof<limb>())), bit & (widthof<limb>()-1));
            b = v;
            // return b?
        }

        void push_back(bool v){
            emplace_back(v);
        }

        void set_bits(word n) noexcept{
            reserve(n);
            bit = n;
        }

        limb* data() noexcept{
            return base.data();
        }

        const limb* data() const noexcept{
            return base.data();
        }

        /**
         * @brief Returns the validity of the vector.
         */
        bool valid() const noexcept{
            return base.valid();
        }

        /**
         * @brief Returns true if empty.
         */
        bool empty() const noexcept{
            return size() == 0;
        }

        ~bit_vector() noexcept = default;

        template<ostream_t T>
        T& print_binary_T(T& os) const{
            if(empty()) return os;

            uint8_t buf[widthof<limb>()];
            size_t buf_idx = 0;


            for(size_t bit_idx = bit; bit_idx-- > 0;){
                size_t limbc = bit_idx >> bit_to_shift<limb>(widthof<limb>());
                size_t bit_in_limb = bit_idx & (widthof<limb>()-1);

                bool chbit = (base[limbc] >> bit_in_limb) & 1;
                buf[buf_idx++] = '0' + chbit;
                if(buf_idx == sizeof(buf)){
                    os.write((const char*)buf, buf_idx);
                    buf_idx = 0;
                }
            }

            if(buf_idx){
                os.write((const char*)buf, buf_idx);
            }

            return os;
        }

        template<ostream_t T>
        friend T& operator<<(T& os, const bit_vector& v){
            return v.print_binary_T<T>(os);
        }
    };

}

#endif // INERTIA_BITVECTOR_HPP
