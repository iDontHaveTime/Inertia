#ifndef INERTIA_INRINT_HPP
#define INERTIA_INRINT_HPP

/**
 * @file inr/Support/inrint.hpp
 * @brief Inertia's arbitrary precision integer class.
 *
 * This header contains Inertia's arbitrary precision integer class.
 *
 **/

#include "inr/Support/inralloc.hpp"
#include "inr/Support/inrcexpr.hpp"
#include "inr/Support/inriterator.hpp"
#include "inr/Support/inrstream.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <ostream>
#include <string_view>


namespace inr{

    /**
     * @brief Inertia's arbitrary precision integer.
     *
     * This integer class can store integers with any amount of bits specified.
     * Can be outputted to a stream (either std::ostream or inr::inr_ostream).
     *
     * It holds an array to uint32_t if heap, but if the bits are less or equal to 64 its on stack.
     * 
     */
    class inrint{
        union{
            uint64_t stack_base;
            uint32_t* heap_base;
        };
        allocator* mem;
        size_t bits;
        /**
         * Bit 0: 0 - on heap, 1 - on stack
         * Bit 1: 0 - unsigned, 1 - signed
         * Bit 2: 0 - original, 1 - temporary
         */
        uint32_t flags;

        void choose_memory(allocator* _mem) noexcept{
            mem = _mem ? _mem : &static_allocator;
        }

        void new_inrint_helper(size_t width){
            flags = 0;
            bits = 0;
            if(width <= widthof<uint64_t>()){
                flags |= 0x1;
                bits = width;
            }
            else{
                size_t bytes = calculate_bytes_up(width);
                heap_base = (uint32_t*)mem->alloc_raw(bytes, alignof(std::max_align_t));
                if(!heap_base){
                    return;
                }
                bits = width;
            }
        }

        inrint(allocator* _mem) noexcept{
            choose_memory(_mem);
        }

        inrint(uint32_t* buffer, size_t width) noexcept{
            choose_memory(nullptr);
            flags = 0x4;
            heap_base = buffer;
            bits = width;
            mask_trailing();
        }

        void from_string_binary(const std::string_view& str) noexcept{
            if(!valid()) return;
            size_t bit = 0;
            for(auto it = str.rbegin(); it != str.rend(); ++it){
                char c = *it;
                bool bit_set;
                if(c == '1'){
                    bit_set = 1;
                }
                else if(c == '0'){
                    bit_set = 0;
                }
                else{
                    return;
                }
                set(bit, bit_set);
                bit++;
                if(bit == bits) return;
            }
            mask_trailing();
        }
    private:
        array_iterator<uint32_t> begin() noexcept{
            return array_iterator(on_stack() ? (uint32_t*)stack_base : heap_base);
        }

        array_iterator<uint32_t> end() noexcept{
            return array_iterator(on_stack() ? (uint32_t*)stack_base + 2 : heap_base + cycles());
        }

    public:

        decltype(bits) get_width() const noexcept{
            return bits;
        }

        /** 
         * @brief Sets the bit to either 1 or 0.
         *
         * @param bit The number of the bit, starting from 0 to width-1.
         * @param v True - 1, False - 0.
         */
        void set(size_t bit, bool v) noexcept{
            if(bit > bits) return;
            if(on_stack()){
                uint64_t mask = (1ULL << bit);
                if(!v){
                    mask = ~mask;
                    stack_base &= mask;
                    return;
                }
                stack_base |= mask;
            }
            else{
                size_t limb = calculate_bytes_up(bit);
                size_t local_bit = bit & 31;
                uint32_t mask = (1u << local_bit);
                if(!v){
                    mask = ~mask;
                    heap_base[limb] &= mask;
                    return;
                }
                heap_base[limb] |= mask;
            }
        }

        /**
         * @brief Gets the binary representation from string.
         *
         * @param str The string to read.
         */
        void from_string_binary(const char* str) noexcept{
            zero_out();
            return from_string_binary(std::string_view(str));
        }

        /**
         * @brief Initializes the integer to this value.
         *
         * @param val The value to initialize it to.
         */
        void from_uint64_t(uint64_t val) noexcept{
            zero_out();
            if(on_stack()){
                stack_base = val;
            }
            else{
                heap_base[0] = (uint32_t)val;
                heap_base[1] = (uint32_t)(val >> 32);
            }
            mask_trailing();
        }

        /**
         * @brief Initializes the integer to this value.
         *
         * @param val The value to initialize it to.
         */
        void from_int(int n) noexcept{
            if(flags & 0x2){
                if(on_stack()){
                    *((int64_t*)&stack_base) = n;
                    mask_trailing();
                }
                else{
                    from_uint64_t((unsigned int)n);
                    bool extend = n < 0;
                    size_t from_limb = sizeof(int) >> 2;
                    size_t n_of_limbs = (bits + 31) >> 5;
                    for(size_t i = from_limb; i < n_of_limbs; i++){
                        heap_base[i] = extend ? -1 : 0;
                    }
                    mask_trailing();
                }
            }
            else{
                unsigned int x = (unsigned int)n;
                from_uint64_t((uint64_t)x);
            }
        }

        /** 
         * @brief Move constructor. Transfers ownership.
         */
        inrint(inrint&& other) noexcept{

            if(other.on_stack()){
                stack_base = other.stack_base;
                other.stack_base = 0;
            }
            else{
                heap_base = other.heap_base;
                other.heap_base = nullptr;
            }
            
            bits = other.bits;
            other.bits = 0;

            flags = other.flags;
            other.flags = 0;

            mem = other.mem;
        }

        /** 
         * @brief Move operator. Transfers ownership.
         */
        inrint& operator=(inrint&& other) noexcept{
            if(this == &other) return *this;

            close();

            if(other.on_stack()){
                stack_base = other.stack_base;
                other.stack_base = 0;
            }
            else{
                heap_base = other.heap_base;
                other.heap_base = nullptr;
            }
            
            bits = other.bits;
            other.bits = 0;

            flags = other.flags;
            other.flags = 0;

            mem = other.mem;

            return *this;
        }

        /**
         * @brief Copy constructor. Deep copy.
         */
        inrint(const inrint& other){
            mem = other.mem;
            new_inrint_helper(other.bits);

            if(on_stack()){
                stack_base = other.stack_base;
            }
            else{
                size_t c = cycles();
                for(size_t i = 0; i < c; i++){
                    heap_base[i] = other.heap_base[i];
                }
                mask_trailing();
            }
        }

        /**
         * @brief Copy operator. Deep copy.
         */
        inrint& operator=(const inrint& other){
            if(this == &other) return *this;

            close();

            mem = other.mem;
            new_inrint_helper(other.bits);

            if(on_stack()){
                stack_base = other.stack_base;
            }
            else{
                size_t c = cycles();
                for(size_t i = 0; i < c; i++){
                    heap_base[i] = other.heap_base[i];
                }
                mask_trailing();
            }

            return *this;
        }

        /**
         * @brief The basic constructor, sets the width of the integer.
         *
         * @param width How many bits should the integer be.
         * @param sign Should the integer be signed.
         * @param _mem Optional, changes the internal allocator.
         */
        explicit inrint(size_t width, bool sign = false, allocator* _mem = nullptr) : inrint(_mem){
            new_inrint_helper(width);
            if(sign){
                flags |= 0x2;
            }
        }

        explicit inrint(const char* _str, size_t width, bool sign = false, allocator* _mem = nullptr) : inrint(width, sign, _mem){
            // TODO
            (void)_str;
        }

        /**
         * @brief Constructor with an int default value.
         *
         * @param n The number to initialize it to.
         * @param width How many bits should the integer be.
         * @param sign Should the integer be signed.
         * @param _mem Optional, changes the internal allocator.
         */
        explicit inrint(int n, size_t width, bool sign = false, allocator* _mem = nullptr) : inrint(width, sign, _mem){
            from_int(n);
        };

        /**
         * @brief The location of this integer.
         * @return True if on stack, false if on heap.
         */
        bool on_stack() const noexcept{
            return flags & 1;
        }

        /**
         * @brief The signedness of the integer.
         * @brief True if signed, false if unsigned.
         */
        bool is_signed() const noexcept{
            return flags & 0x2;
        }

        /**
         * @brief Bytes it takes up.
         * @return The amount of bytes the integer value takes up.
         */
        size_t capacity() const noexcept{
            return on_stack() ? sizeof(stack_base) : calculate_bytes_up(bits);
        }

        /**
         * @brief The amount of cycles to iterate through the array.
         * @return Amount of uint32_t allocated.
         */
        size_t cycles() const noexcept{
            return capacity() >> 2;
        }

        /**
         * @brief Returns validity of the integer.
         * @return True if valid, false if not.
         */
        bool valid() const noexcept{
            if(bits == 0) return false;
            return on_stack() ? true : (heap_base ? true : false);
        }

        /**
         * @brief Frees the integer correctly.
         */
        ~inrint(){
            close();
        }

        /**
         * @brief Returns integer's allocator.
         * @return Pointer to allocator.
         */
        allocator* get_allocator() const noexcept{
            return mem;
        }

        /**
         * @brief Frees the integer.
         */
        void close() noexcept{
            if(flags & 0x4) return;
            if(!on_stack()){
                mem->free_raw((void*)heap_base, capacity());
            }
            bits = 0;
            flags = 0;
        }

        /**
         * @brief Sets all bits to 1 or 0.
         * @param c True - 1, false - 0.
         */
        void set_all(bool c) noexcept{
            if(on_stack()){
                stack_base = c ? -1 : 0;
            }
            else{
                size_t cycles_count = cycles();
                for(size_t i = 0; i < cycles_count; i++){
                    heap_base[i] = c ? -1 : 0;
                }
            }
        }

        /**
         * @brief Zeroes out the integer.
         */
        void zero_out() noexcept{
            set_all(false);
        }

        /**
         * @brief Adds the other inrint to this one.
         *
         * @param other The 'inrint' to add to this one.
         */
        void add(const inrint& other) noexcept;

        /**
         * @brief Shifts left the inrint N times.
         *
         * @param n Times to bitshift
         */
        void lbit_shift(size_t n) noexcept;

        /**
         * @brief Shifts right the inrint N times.
         *
         * @param n Times to bitshift
         */
        void rbit_shift(size_t n) noexcept;

        /**
         * @brief Masks the trailing bits.
         */
        void mask_trailing() noexcept{
            if(on_stack()){
                if(bits < 64){
                    stack_base &= (1ULL << bits) - 1ULL;
                }
            }
            else{
                // >> 3 = / 8, >> 4 = / 16, >> 5 = / 32 
                uint32_t bit = bits & 31;
                if(!bit) return;
                heap_base[cycles()-1] &= (1u << bit) - 1u;
            }
        }

    private:

        /**
         * @brief Uses the double-dabble algorithm to print out decimal.
         * @param os Stream to output it to. Must follow write(data, n) format.
         */
        template<typename T>
        T& print_decimal_T(T& os) const{
            if(!valid()) return os;

            if(on_stack()){
                if(flags & 0x2){
                    uint64_t stack_cpy = stack_base;
                    if(bits < widthof<uint64_t>() && (stack_base >> (bits - 1)) & 1){
                        stack_cpy |= ~((1ULL << bits) - 1);
                    }
                    return os<<(int64_t)stack_cpy;
                }
                else{
                    return os<<stack_base;
                }
            }

            size_t decimal_digits = (bits * 643 + 2135) / 2136;
            void* decimal_buffer_and_apint = mem->alloc_raw(decimal_digits + capacity(), alignof(inrint));

            void* apint_limbs = decimal_buffer_and_apint;
            uint8_t* decimal_buffer = (uint8_t*)((char*)decimal_buffer_and_apint + capacity());

            memcpy(apint_limbs, heap_base, capacity());
            inrint temp_int((uint32_t*)apint_limbs, bits);

            memset(decimal_buffer, 0, decimal_digits);

            bool is_negative = false;
            if(flags & 0x2){
                size_t msb_idx = bits - 1;
                size_t limb = msb_idx >> 5;
                size_t bit = msb_idx & 31;

                is_negative = (temp_int.heap_base[limb] >> bit) & 1;

                uint32_t carry = 1;
                size_t n = temp_int.cycles();

                for(size_t i = 0; i < n; i++){
                    uint64_t value = (uint64_t)(~temp_int.heap_base[i]) + carry;
                    temp_int.heap_base[i] = (uint32_t)value;
                    carry = (uint32_t)(value >> 32);
                }
            }

            for(size_t bit_idx = bits; bit_idx-- > 0;){
                for(size_t d = 0; d < decimal_digits; d++){
                    if(decimal_buffer[d] >= 5){
                        decimal_buffer[d] += 3; 
                    }
                }

                size_t limb = bit_idx >> 5;
                size_t bit = bit_idx & 31;
                uint8_t b = (temp_int.heap_base[limb] >> bit) & 1;

                uint8_t carry = b;

                for(size_t d = decimal_digits; d-- > 0;){
                    uint8_t next_carry = (decimal_buffer[d] & 0x8) >> 3;
                    decimal_buffer[d] = ((decimal_buffer[d] << 1) & 0xF) | carry;
                    carry = next_carry;
                }
            }

            bool leading = true;
            const char* start_of_decimal = nullptr;
            size_t size_of_decimal_output = 0;
            
            for(size_t i = 0; i < decimal_digits; i++){
                uint8_t val = decimal_buffer[i];

                decimal_buffer[i] = val + '0';

                if(val != 0 || !leading || i == decimal_digits - 1){
                    if(leading){
                        start_of_decimal = (const char*)&decimal_buffer[i];
                        size_of_decimal_output = decimal_digits - i;
                        leading = false;
                    }
                }
            }

            if(leading){
                start_of_decimal = (const char*)&decimal_buffer[decimal_digits - 1]; 
                size_of_decimal_output = 1;
            }

            if(is_negative){
                os<<'-';
            }
            os.write(start_of_decimal, size_of_decimal_output);
            
            mem->free_raw(decimal_buffer_and_apint, decimal_digits + capacity());
            return os;
        }

        template<typename T>
        T& print_binary_T(T& os) const{
            if(!valid()) return os;
            uint8_t buf[64];
            size_t buf_idx = 0;

            if(on_stack()){
                for(size_t i = bits; i-- > 0;){
                    bool bit = (stack_base >> i) & 1;
                    buf[buf_idx++] = '0' + bit;
                    if(buf_idx == sizeof(buf)){
                        os.write((const char*)buf, buf_idx);
                        buf_idx = 0;
                    }
                }
            }
            else{
                size_t total_bits = bits;
                for(size_t bit_idx = total_bits; bit_idx-- > 0;){
                    size_t limb = bit_idx >> 5;
                    size_t bit_in_limb = bit_idx & 31;
                    bool bit = (heap_base[limb] >> bit_in_limb) & 1;
                    buf[buf_idx++] = '0' + bit;
                    if(buf_idx == sizeof(buf)){
                        os.write((const char*)buf, buf_idx);
                        buf_idx = 0;
                    }
                }
            }

            if(buf_idx){
                os.write((const char*)buf, buf_idx);
            }

            return os;
        }
    public:

        /** 
         * @brief Prints out binary of the integer to std::ostream.
         * @param os The STL's stream.
         */
        std::ostream& print_binary(std::ostream& os) const{
            return print_binary_T(os);
        }

        /** 
         * @brief Prints out binary of the integer to inr::inr_ostream.
         * @param os The Inertia's stream.
         */
        inr::inr_ostream& print_binary(inr::inr_ostream& os) const{
            return print_binary_T(os);
        }

        /** 
         * @brief Prints out decimal of the integer to std::ostream.
         * @param os The STL's stream.
         */
        std::ostream& print_decimal(std::ostream& os) const{
            return print_decimal_T(os);
        }

        /** 
         * @brief Prints out decimal of the integer to inr::inr_ostream.
         * @param os The Inertia's stream.
         */
        inr::inr_ostream& print_decimal(inr::inr_ostream& os) const{
            return print_decimal_T(os);
        }

        /**
         * @brief Bit shifts left N times.
         * @param n Times to bit shift.
         */
        inrint& operator<<=(size_t n) noexcept{
            lbit_shift(n);
            return *this;
        }

        /**
         * @brief Copies and shifts the integer.
         */
        inrint operator<<(size_t n){
            inrint cpy(*this);
            cpy <<= n;
            return cpy;
        }

        /**
         * @brief Bit shifts right N times.
         * @param n Times to bit shift.
         */
        inrint& operator>>=(size_t n) noexcept{
            rbit_shift(n);
            return *this;
        }

        /**
         * @brief Copies and shifts the integer.
         */
        inrint operator>>(size_t n){
            inrint cpy(*this);
            cpy >>= n;
            return cpy;
        }

        /**
         * @brief Adds another 'inrint' to this one.
         * @param other The other one to add to this one.
         */
        inrint& operator+=(const inrint& other) noexcept{
            add(other);
            return *this;
        }

        /**
         * @brief Adds N to the current integer.
         * @param n The number to add.
         */
        inrint& operator+=(uint64_t n) noexcept{
            inrint temp(widthof<uint64_t>(), is_signed(), mem);
            temp.from_uint64_t(n);
            (*this) += temp;
            return *this;
        }

        friend std::ostream& operator<<(std::ostream& os, const inrint& integer){
            return integer.print_decimal(os);
        }

        friend inr_ostream& operator<<(inr_ostream& os, const inrint& integer){
            return integer.print_decimal(os);
        }

        /**
         * @brief Prefix increment operator.
         */
        inrint& operator++() noexcept{
            (*this) += 1;
            return *this;
        }

        /**
         * @brief Postfix increment operator. 
         */
        inrint operator++(int) noexcept{
            inrint cpy(*this);
            ++(*this);
            return cpy;
        }

        /**
         * @brief Assignment operator.
         */
        inrint& operator=(uint64_t n) noexcept{
            from_uint64_t(n);
            return *this;
        }

        /**
         * @brief Does ~inrint (not the destructor, the bitwise not) basically.
         */
        void bit_not() noexcept{
            if(on_stack()){
                stack_base = ~stack_base;
            }
            else{
                size_t cycles_count = cycles();
                for(size_t i = 0; i < cycles_count; i++){
                    heap_base[i] = ~heap_base[i];
                }
            }
            mask_trailing();
        }

        /**
         * @brief Bitwise OR with another 'inrint'.
         */
        void bit_or(const inrint& other) noexcept{
            if(on_stack() && other.on_stack()){
                stack_base |= other.stack_base;
            }
            else if(on_stack() && !other.on_stack()){
                stack_base |= *(uint64_t*)heap_base;
            }
            else{
                if(other.on_stack()){
                    *((uint64_t*)heap_base) |= other.stack_base;
                }
                else{
                    size_t min_cycle = std::min(cycles(), other.cycles());
                    for(size_t i = 0; i < min_cycle; i++){
                        heap_base[i] |= other.heap_base[i];
                    }
                }
            }
        }
    };

}

#endif // INERTIA_INRINT_HPP
