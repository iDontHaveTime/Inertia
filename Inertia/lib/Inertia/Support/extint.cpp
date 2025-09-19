#include <Inertia/Support/extint.hpp>

constexpr size_t u64_w = sizeof(uint64_t)*8;

namespace Inertia{

void extint64::shift_left() noexcept{
    uint64_t carry = 0;
    for(size_t i = 0; i < allocated; i++){
        uint64_t word = array[i];
        uint64_t new_carry = word >> (u64_w-1);

        word = (word << 1) | carry;
        array[i] = word;

        carry = new_carry;
    }

    mask_upper_bits();
}

void extint64::add_u64(uint64_t n) noexcept{
    uint64_t carry = n;
    for(size_t i = 0; i < allocated && carry != 0; i++){
        carry = __builtin_add_overflow(array[i], carry, &array[i]);
    }
}

void extint64::add_extint(const extint64& n) noexcept{
    uint64_t carry = 0;
    size_t minimum = n.allocated < allocated ? n.allocated : allocated;
    for(size_t i = 0; i < minimum; i++){
        carry = __builtin_add_overflow(array[i], n.array[i] + carry, &array[i]);
    }

    for(size_t i = minimum; i < allocated && carry != 0; i++){
        carry = __builtin_add_overflow(array[i], carry, &array[i]);
    }
}

void extint64::sub_extint(const extint64& n) noexcept{
    uint64_t carry = 1;
    size_t minimum = n.allocated < allocated ? n.allocated : allocated;

    for(size_t i = 0; i < minimum; i++){
        uint64_t neg_limb = ~n.array[i];
        carry = __builtin_add_overflow(neg_limb, carry, &neg_limb);
        carry = __builtin_add_overflow(array[i], neg_limb, &array[i]);
    }

    for(size_t i = minimum; i < allocated && carry != 0; i++){
        carry = __builtin_add_overflow(array[i], carry, &array[i]);
    }

    mask_upper_bits();
}

void extint64::sub_u64(uint64_t n) noexcept{
    uint64_t carry = 1;

    for(size_t i = 0; i < allocated; i++){
        uint64_t b = (i == 0) ? n : 0;
        uint64_t neg_limb = ~b;

        uint64_t temp;
        carry = __builtin_add_overflow(neg_limb, carry, &temp);
        carry = __builtin_add_overflow(array[i], temp, &array[i]);
    }

    mask_upper_bits();
}

}