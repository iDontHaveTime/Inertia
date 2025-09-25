#ifndef INERTIA_EXTINT_HPP
#define INERTIA_EXTINT_HPP

#include <ostream>
#include <cstdint>
#include <cstddef>
#include <cstring>

#ifndef UINT64_WIDTH
#define UINT64_WIDTH 64
#define UINT64_WIDTH_DEFINED_INSIDE
#endif

namespace Inertia{

class extint64{
    uint64_t* array;
    uint32_t bits;
    uint32_t allocated;

    void new_extint(uint32_t bc){
        if(bc == 0){
            array = nullptr;
            bits = 0;
            allocated = 0;
            return;
        }

        allocated = (bc + (UINT64_WIDTH-1)) / UINT64_WIDTH;
        array = new uint64_t[allocated]();
        bits = bc;
    }

    inline void mask_upper_bits() noexcept{
        size_t msw_bits = bits - (allocated - 1) * UINT64_WIDTH;
        if(msw_bits < UINT64_WIDTH){
            array[allocated-1] &= (1ULL << msw_bits) - 1;
        }
    }
public:

    extint64() = delete;

    extint64(uint32_t bit_count, uint64_t starting_lower_bits = 0){
        new_extint(bit_count);
        if(starting_lower_bits != 0){
            array[0] = starting_lower_bits;
            if(bits < UINT64_WIDTH){
                uint64_t mask = (1ULL << bits)-1;
                array[0] &= mask;
            }
        }
    }

    extint64(const extint64& other){
        bits = other.bits;
        allocated = other.allocated;
        array = new uint64_t[allocated];
        memcpy(array, other.array, allocated*sizeof(uint64_t));
    }

    extint64& operator=(const extint64& other){
        if(this == &other) return *this;

        delete[] array;

        bits = other.bits;
        allocated = other.allocated;
        array = new uint64_t[allocated];
        memcpy(array, other.array, allocated*sizeof(uint64_t));
        return *this;
    }

    extint64(extint64&& other) noexcept{
        allocated = other.allocated;
        bits = other.bits;
        array = other.array;

        other.allocated = 0;
        other.bits = 0;
        other.array = nullptr;
    }

    extint64& operator=(extint64&& other) noexcept{
        if(this == &other) return *this;

        delete[] array;

        allocated = other.allocated;
        bits = other.bits;
        array = other.array;

        other.allocated = 0;
        other.bits = 0;
        other.array = nullptr;
        return *this;
    }

    uint64_t* get_array() const noexcept{
        return array;
    }

    uint32_t get_bits() const noexcept{
        return bits;
    }

    uint32_t get_allocated() const noexcept{
        return allocated;
    }

    void zero_out() noexcept{
        memset(array, 0, allocated*sizeof(uint64_t));
    }

    void add_u64(uint64_t) noexcept;
    void add_extint(const extint64&) noexcept;

    void sub_u64(uint64_t) noexcept;
    void sub_extint(const extint64&) noexcept;

    void shift_left() noexcept;
    void shift_left(unsigned int n) noexcept{
        while(n--){
            shift_left();
        }
    }
    void bitwise_not() noexcept{
        for(size_t i = 0; i < allocated; i++){
            array[i] = ~array[i];
        }
        mask_upper_bits();
    }
    void negate() noexcept{
        bitwise_not();
        add_u64(1);
    }

    void operator<<=(unsigned int n) noexcept{
        if(n >= bits){
            zero_out();
            return;
        }
        shift_left(n);
    }

    extint64 operator<<(unsigned int n) const{
        extint64 temp(*this);

        temp.shift_left(n);

        return temp;
    }

    extint64 operator+(const extint64& other) const{
        extint64 temp(*this);

        temp.add_extint(other);

        return temp;
    }

    extint64& operator+=(const extint64& other) noexcept{
        add_extint(other);
        return *this;
    }

    extint64 operator+(uint64_t n) const{
        extint64 temp(*this);

        temp.add_u64(n);

        return temp;
    }

    extint64& operator+=(uint64_t n) noexcept{
        add_u64(n);
        return *this;
    }

    extint64 operator-() const{
        extint64 temp(*this);
        temp.negate();
        return temp;
    }

    extint64 operator-(const extint64& other) const{
        extint64 temp(*this);

        temp.sub_extint(other);

        return temp;
    }

    extint64& operator-=(const extint64& other) noexcept{
        sub_extint(other);
        return *this;
    }

    extint64 operator-(uint64_t n) const{
        extint64 temp(*this);

        temp.sub_u64(n);

        return temp;
    }

    extint64& operator-=(uint64_t n) noexcept{
        sub_u64(n);
        return *this;
    }

    // max is bits-1, so like 128 bits means you can have 0-127 positions
    void set_bit(uint32_t bit, bool b) noexcept{
        if(bit >= bits) return;
        uint32_t word = bit >> 6;
        bit = bit & (UINT64_WIDTH-1);
        if(b){
            array[word] |= 1ULL << bit;
        }
        else{
            array[word] &= ~(1ULL << bit);
        }
    }

    bool get_bit(uint32_t bit) const noexcept{
        if(bit >= bits) return false;
        uint32_t word = bit >> 6;
        bit = bit & (UINT64_WIDTH-1);
        return (array[word] >> bit) & 1ULL;
    }

    ~extint64() noexcept{
        delete[] array;
        bits = 0;
        allocated = 0;
    }

    class bit_iterator{
        const extint64* ext;
        uint32_t pos;
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = bool;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = bool;

        bit_iterator(const extint64* ex, uint32_t p) noexcept : ext(ex), pos(p){};

        bool operator*() const noexcept{
            uint32_t word = pos / UINT64_WIDTH;
            uint32_t bit = pos % UINT64_WIDTH;
            return (ext->array[word] >> bit) & 1ULL;
        }
        bit_iterator& operator++() noexcept{
            pos++;
            return *this;
        }
        bit_iterator operator++(int) noexcept{
            bit_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const bit_iterator& other) const noexcept{
            return ext == other.ext && pos == other.pos;
        }
        bool operator!=(const bit_iterator& other) const noexcept{
            return !(*this == other);
        }
    };

    bit_iterator begin() const noexcept{
        return {this, 0};
    }

    bit_iterator end() const noexcept{
        return {this, bits};
    }

    std::ostream& print_binary(std::ostream& os) const noexcept{
        if(!bits) return os;
        for(int i = bits - 1; i >= 0; i--){
            uint32_t word = i / UINT64_WIDTH;
            uint32_t bit = i % UINT64_WIDTH;
            os<<((array[word] >> bit) & 1ULL);
        }  
        return os;
    }
};

using extint = extint64;

}


#ifdef UINT64_WIDTH_DEFINED_INSIDE
#undef UINT64_WIDTH_DEFINED_INSIDE
#undef UINT64_WIDTH
#endif

#endif // INERTIA_EXTINT_HPP
